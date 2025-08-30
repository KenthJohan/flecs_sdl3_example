#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + NAME_MAX + 1))

static void print_events(uint32_t e)
{
	if (e & IN_ACCESS)
		printf("IN_ACCESS ");
	if (e & IN_MODIFY)
		printf("IN_MODIFY ");
	if (e & IN_CLOSE_WRITE)
		printf("IN_CLOSE_WRITE ");
	if (e & IN_CLOSE_NOWRITE)
		printf("IN_CLOSE_NOWRITE ");
	if (e & IN_OPEN)
		printf("IN_OPEN ");
	if (e & IN_MOVED_FROM)
		printf("IN_MOVED_FROM ");
	if (e & IN_MOVED_TO)
		printf("IN_MOVED_TO ");
	if (e & IN_CREATE)
		printf("IN_CREATE ");
	if (e & IN_DELETE)
		printf("IN_DELETE ");
	if (e & IN_DELETE_SELF)
		printf("IN_DELETE_SELF ");
	if (e & IN_MOVE_SELF)
		printf("IN_MOVE_SELF ");
}

int main1(int argc, char *argv[])
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <dir1> <dir2>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int inotify_fd = inotify_init1(IN_NONBLOCK);
	if (inotify_fd == -1) {
		perror("inotify_init1");
		exit(EXIT_FAILURE);
	}

	int wd1 = inotify_add_watch(inotify_fd, argv[1], IN_ALL_EVENTS);
	int wd2 = inotify_add_watch(inotify_fd, argv[2], IN_ALL_EVENTS);
	if (wd1 == -1 || wd2 == -1) {
		perror("inotify_add_watch");
		close(inotify_fd);
		exit(EXIT_FAILURE);
	}

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("epoll_create1");
		close(inotify_fd);
		exit(EXIT_FAILURE);
	}

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = inotify_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, inotify_fd, &event) == -1) {
		perror("epoll_ctl");
		close(epoll_fd);
		close(inotify_fd);
		exit(EXIT_FAILURE);
	}

	printf("Monitoring %s (wd=%d) and %s (wd=%d)...\n", argv[1], wd1, argv[2], wd2);

	while (1) {
		struct epoll_event events[MAX_EVENTS];
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			if (errno == EINTR)
				continue;
			perror("epoll_wait");
			break;
		}

		for (int i = 0; i < nfds; ++i) {
			if (events[i].data.fd == inotify_fd) {
				char buffer[BUF_LEN];
				ssize_t len = read(inotify_fd, buffer, BUF_LEN);
				if (len < 0 && errno != EAGAIN) {
					perror("read");
					break;
				}

				ssize_t pos = 0;
				while (pos < len) {
					struct inotify_event *ev = (struct inotify_event *)&buffer[pos];
					printf("Event: wd=%d mask=0x%x cookie=%u len=%u, ",
					ev->wd, ev->mask, ev->cookie, ev->len);
					print_events(ev->mask);
					if (ev->len > 0)
						printf(" name=%s", ev->name);
					printf(" [dir: %s]\n", ev->wd == wd1 ? argv[1] : argv[2]);
					pos += EVENT_SIZE + ev->len;
				}
			}
		}
	}

	close(epoll_fd);
	close(inotify_fd);

	return 0;
}

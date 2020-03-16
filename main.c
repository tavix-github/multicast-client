
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <getopt.h>

static char multicast_ip[32] = "239.0.0.10"; // 239.0.0.0~239.255.255.255
static short remote_port = 9812; // remote port to listened
static char interface_ip[32] = "0.0.0.0"; // interface ip to sent datagram
static size_t max_frame = 2048;

static const char* short_options = "m:i:p:l:h"; // Short options
static const struct option long_options[] = { // Long options
    {"multicast", 	required_argument, 	NULL, 'm'},
    {"port", 	    required_argument,	NULL, 'p'},
    {"interface", 	required_argument, 	NULL, 'i'},
    {"maxlen", 	    required_argument, 	NULL, 'l'},
    {"help", 	    no_argument, 		NULL, 'h'},
	{ 0, 		    0, 					0, 		0}
};

static const char *help = "\nUsage: multicast-client [option] [argument]\n\n"
        "\t-m, --multicast\tmulticast ip, 239.0.0.0~239.255.255.255, default: 239.0.0.10\n"
        "\t-p, --port\tremote udp port, default: 9812\n"
        "\t-i, --interface\tinterface ip to sent datagram, default: 0.0.0.0\n"
		"\t-l, --maxlen\tmax udp frame length, default: 2048\n"
		"\t-h, --help\tdisplay this help and exit\n";

int main(int argc, char *argv[])
{

    int retval = 0;

    /* Command line parameter analysis */
	/* ***********************************************************
	*		***	    Command line parameter analysis       ***	 * 
	* ***********************************************************/
	int c;
	while((c = getopt_long (argc, argv, short_options, long_options, NULL)) != -1)
	{
		switch (c)
		{
			case 'm': // --multicast
				strncpy(multicast_ip, optarg, strlen(optarg) +1);
				break;
            case 'i': // --interface
				strncpy(interface_ip, optarg, strlen(optarg) +1);
				break;
			case 'p': // --port
				remote_port = strtoul(optarg, NULL, 0);
				break;
			case 'l': // --maxlen
				max_frame = strtoul(optarg, NULL, 0);
				break;
			case 'h': // --help
				printf("%s\n", help); // Display help
				return 0;
			case '?':
				return 0;
		};
	};

    /* crate socket for udp */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket failed");
        return EXIT_FAILURE;
    }

    /* bind local interface and port */
    struct sockaddr_in local_addr ; /* local ip addr and port to sent datagram */
    local_addr.sin_family = AF_INET ; /* IPv4 */
    local_addr.sin_port = htons(0) ; /* local port */
    local_addr.sin_addr.s_addr = inet_addr(interface_ip); /* local computer ip */
    retval = bind(sock_fd,(struct sockaddr *)&local_addr,sizeof(local_addr));
    if (retval != 0) {
        perror("failed to bind local inteface");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in from_addr; /* remote ip addr and port */
    socklen_t fromlen = sizeof(from_addr);
    from_addr.sin_family = AF_INET; /* IPv4 */
    from_addr.sin_port = htons(remote_port) ; /* remote port */
    from_addr.sin_addr.s_addr = inet_addr(multicast_ip); /* remote computer ip */

    /* request buffer */
    char *buf = malloc(max_frame);
    if (buf == NULL) {
        perror("failed to request buffer");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    ssize_t rbytes = 0, sbytes = 0;
    while (1) {
        scanf("%s", buf);

        sbytes = sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *)&from_addr, sizeof(struct sockaddr_in));
        if (sbytes > 0) {
            printf("%s send ok\n", buf);
        }
    }

    free(buf);
    close(sock_fd);

    return 0; 
}
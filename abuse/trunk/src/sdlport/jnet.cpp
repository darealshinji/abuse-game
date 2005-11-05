#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#include "jnet.hpp"
#include "macs.hpp"
#include "jmalloc.hpp"

int current_sock_err;

int net_init( int protocol )
{
	printf( "YO!\n" );
	if( protocol == TCPIP_PROTOCOL )
		return 1;
	else
		return 0;
}

void net_uninit()
{
	// Nothing to do
}

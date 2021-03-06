#ifndef _SYS_SYSLOG_H
#define _SYS_SYSLOG_H

#ifndef WIN32
	#include <sys/syslog.h>
#else
	#include <stdarg.h>

	#define	LOG_EMERG	0	/* system is unusable */
	#define	LOG_ALERT	1	/* action must be taken immediately */
	#define	LOG_CRIT	2	/* critical conditions */
	#define	LOG_ERR		3	/* error conditions */
	#define	LOG_WARNING	4	/* warning conditions */
	#define	LOG_NOTICE	5	/* normal but significant condition */
	#define	LOG_INFO	6	/* informational */
	#define	LOG_DEBUG	7	/* debug-level messages */

	#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
	/* extract priority */
	#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
	#define	LOG_MAKEPRI(fac, pri)	(((fac) << 3) | (pri))

	/* facility codes */
	#define	LOG_KERN	(0<<3)	/* kernel messages */
	#define	LOG_USER	(1<<3)	/* random user-level messages */
	#define	LOG_MAIL	(2<<3)	/* mail system */
	#define	LOG_DAEMON	(3<<3)	/* system daemons */
	#define	LOG_AUTH	(4<<3)	/* security/authorization messages */
	#define	LOG_SYSLOG	(5<<3)	/* messages generated internally by syslogd */
	#define	LOG_LPR		(6<<3)	/* line printer subsystem */
	#define	LOG_NEWS	(7<<3)	/* network news subsystem */
	#define	LOG_UUCP	(8<<3)	/* UUCP subsystem */
	#define	LOG_CRON	(9<<3)	/* clock daemon */
	#define	LOG_AUTHPRIV	(10<<3)	/* security/authorization messages (private) */
	#define	LOG_FTP		(11<<3)	/* ftp daemon */

	/* other codes through 15 reserved for system use */
	#define	LOG_LOCAL0	(16<<3)	/* reserved for local use */
	#define	LOG_LOCAL1	(17<<3)	/* reserved for local use */
	#define	LOG_LOCAL2	(18<<3)	/* reserved for local use */
	#define	LOG_LOCAL3	(19<<3)	/* reserved for local use */
	#define	LOG_LOCAL4	(20<<3)	/* reserved for local use */
	#define	LOG_LOCAL5	(21<<3)	/* reserved for local use */
	#define	LOG_LOCAL6	(22<<3)	/* reserved for local use */
	#define	LOG_LOCAL7	(23<<3)	/* reserved for local use */

	#define	LOG_NFACILITIES	24	/* current number of facilities */
	#define	LOG_FACMASK	0x03f8	/* mask to extract facility part */
	/* facility of pri */
	#define	LOG_FAC(p)	(((p) & LOG_FACMASK) >> 3)

	/*
	 * arguments to setlogmask.
	 */
	#define	LOG_MASK(pri)	(1 << (pri))		/* mask for one priority */
	#define	LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */

	/*
	 * Option flags for openlog.
	 *
	 * LOG_ODELAY no longer does anything.
	 * LOG_NDELAY is the inverse of what it used to be.
	 */
	#define	LOG_PID		0x01	/* log the pid with each message */
	#define	LOG_CONS	0x02	/* log on the console if errors in sending */
	#define	LOG_ODELAY	0x04	/* delay open until first syslog() (default) */
	#define	LOG_NDELAY	0x08	/* don't delay open */
	#define	LOG_NOWAIT	0x10	/* don't wait for console forks: DEPRECATED */
	#define	LOG_PERROR	0x20	/* log to stderr as well */

	#define SYSLOG_PORT     514

	/* Close desriptor used to write to system logger.  */
	void closelog(void);

	/* Open connection to system logger.  */
	void openlog(char* __ident, int __option, int __facility);

	/* Set the log mask level.  */
	int setlogmask(int __mask);

	/* Generate a log message using FMT string and option arguments.  */
	void syslog(int __pri, char* __fmt, ...);

	/* Generate a log message using FMT and using arguments pointed to by AP.  */
	void vsyslog(int __pri, char* __fmt, va_list __ap);

	void exit_syslog(void);

#endif

void init_syslog(const char* hostname);

#endif /* syslog.h */


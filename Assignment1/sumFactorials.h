/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SUMFACTORIALS_H_RPCGEN
#define _SUMFACTORIALS_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct number {
	int x;
};
typedef struct number number;

#define SUM_PROG 0x12345678
#define SUM_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define sumFactorial 1
extern  long * sumfactorial_1(number *, CLIENT *);
extern  long * sumfactorial_1_svc(number *, struct svc_req *);
extern int sum_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define sumFactorial 1
extern  long * sumfactorial_1();
extern  long * sumfactorial_1_svc();
extern int sum_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_number (XDR *, number*);

#else /* K&R C */
extern bool_t xdr_number ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_SUMFACTORIALS_H_RPCGEN */

/* Temporary ARM64 link veneers.  These establish the symbols required by the
   runtime while the full TLS signal-stack transition is being ported. */
	.text
	.p2align 2
	.global sigdelayed
	.global _sigbe
	.global _sigdelayed_end
sigdelayed:
_sigbe:
	ret
_sigdelayed_end:

	/* sigsetjmp/siglongjmp, mirroring the x86_64 versions in gendef.
	   The AArch64 newlib jmp_buf (_JBLEN 22, see machine/setjmp.h) holds
	   x19-x30, sp and d8-d15 in its first 176 bytes; sigjmp_buf appends
	   the savemask flag at byte 176 (_SAVEMASK) and the saved sigset_t
	   at byte 184 (_SIGMASK).

	   Keep the ABI exports out of the early loader text. */
	.section .text.zsigsetjmp,"xr"
	.p2align 2
	.global sigsetjmp
sigsetjmp:
	str	w1, [x0, #176]		/* buf[_SAVEMASK] = savemask */
	cbz	w1, 1f			/* mask not requested, plain setjmp */
	stp	x0, x30, [sp, #-16]!
	add	x2, x0, #184		/* oset = &buf[_SIGMASK] */
	mov	x1, xzr			/* set = NULL */
	mov	w0, #0			/* how = SIG_SETMASK */
	bl	pthread_sigmask
	ldp	x0, x30, [sp], #16
1:	b	setjmp

	.global siglongjmp
siglongjmp:
	ldr	w2, [x0, #176]		/* buf[_SAVEMASK] set? */
	cbz	w2, 1f			/* no, plain longjmp */
	stp	x0, x1, [sp, #-32]!
	str	x30, [sp, #16]
	add	x1, x0, #184		/* set = &buf[_SIGMASK] */
	mov	x2, xzr			/* oset = NULL */
	mov	w0, #0			/* how = SIG_SETMASK */
	bl	pthread_sigmask
	ldr	x30, [sp, #16]
	ldp	x0, x1, [sp], #32
1:	b	longjmp

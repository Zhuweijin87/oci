#ifndef _EMP_H
#define _EMP_H

typedef struct emp_s
{
	int		empno;
	char	ename[11];
	char	job[10];
	int		mgr;
	double	sal;
	double	comm;
	int		deptno;
} Emp;

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oci.h"
#include "emp.h"

/* oracle 自带数据库 */
char	select_sql[] = "select empno, ename, job, mgr, sal, comm, deptno from emp";
char	update_sql[] = "";
char	insert_sql[] = "";

void PrintError(char *msg, OCIError *error)
{
    sb4     errCode;
    char    errmsg[100] = {'\0'};
    OCIErrorGet((dvoid *)error, (ub4)1, NULL, &errCode, (text *)errmsg, (ub4)sizeof(errmsg), OCI_HTYPE_ERROR);
    fprintf(stderr, "%s %s\n", msg, errmsg);
}

int query_fetch_by_one(OCISvcCtx *context, OCIError *error)
{
	int			ret;
	OCIStmt		*stmt = NULL;
	
	ret = OCIStmtPrepare2(context, &stmt, error, (text *)select_sql, strlen(select_sql), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
	if(ret)
	{
		PrintError("OCIStmtPrepare2 error: ", error);
		return -1;
	}	

	/* 绑定输入参数 */
#if 0
	OCIBind		*bind1 = NULL;
	
	OCIBindByName(stmt, &bind1, error, (text *)"", -1, (void *), sizeof(), SQLT_INT, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
#endif

	//OCIBinfByName(stmt, &bind1, error, 0, 0, );
	ret = OCIStmtExecute(context, stmt, error, 0, 0, (OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtExecute error: ", error);
		return -1;
	}

	/* 定义输出参数 */
	OCIDefine	*def1 = NULL;
	OCIDefine 	*def2 = NULL;
	OCIDefine 	*def3 = NULL;
	OCIDefine	*def4 = NULL;
	OCIDefine	*def5 = NULL;
	OCIDefine	*def6 = NULL;
	OCIDefine	*def7 = NULL;

	int         empno_ind;
    int         ename_ind;
    int         job_ind;
    int         mgr_ind;
    int         sal_ind;
    int         comm_ind;
    int         deptno_ind;

	Emp		emp;

	OCIDefineByPos(stmt, &def1, error, 1, (void *)&emp.empno, sizeof(emp.empno), SQLT_INT, (dvoid *)&empno_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def2, error, 2, (void *)emp.ename, sizeof(emp.ename), SQLT_STR, (dvoid *)&ename_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def3, error, 3, (void *)emp.job, sizeof(emp.job), SQLT_STR, (dvoid *)&job_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def4, error, 4, (void *)&emp.mgr, sizeof(emp.mgr), SQLT_INT, (dvoid *)&mgr_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def5, error, 5, (void *)&emp.sal, sizeof(emp.sal), SQLT_FLT, (dvoid *)&sal_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def6, error, 6, (void *)&emp.comm, sizeof(emp.comm), SQLT_FLT, (dvoid *)&comm_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def6, error, 7, (void *)&emp.deptno, sizeof(emp.deptno), SQLT_INT, (dvoid *)&deptno_ind, NULL, NULL, OCI_DEFAULT);

	int		done = 0, i;
	int		status = 0, rows = 0;
	while(!done)
	{
		status = OCIStmtFetch(stmt, error, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
		if(status == OCI_SUCCESS || status == OCI_NO_DATA)
		{
			if(status == OCI_SUCCESS )	
			{
				rows = 1;
			}
		
			if(status == OCI_NO_DATA)
			{
				ret = OCIAttrGet(stmt, OCI_HTYPE_STMT, &rows, NULL, OCI_ATTR_ROWS_FETCHED, error);
				if(ret != OCI_SUCCESS)
					PrintError("OCIAttrGet OCI_HTYPE_STMT: ", error);
				done = 1;
			}
		
			for(i = 0; i<rows; i++)
			{
				/* 打印查询数据 */
				printf("empno:%d, ename:%s, job:%s, mgr:%d, sal:%.2f, comm:%.2f, depno: %d\n", 
					emp.empno, emp.ename, emp.job, emp.mgr, emp.sal, emp.comm, emp.deptno);
			}
		}
		else
		{
			printf("Error: %d\n", status);
			PrintError("Fetch stmt fail, ", error);
			done = 1;
		}
	}

	OCIStmtRelease(stmt, error, NULL, 0, OCI_DEFAULT);

	return 0;
}

int query_fetch_by_multi(OCISvcCtx *context, OCIError *error)
{
	int			ret;
	
	OCIStmt		*stmt;
	
	ret = OCIStmtPrepare2(context, &stmt, error, (text *)select_sql, strlen(select_sql), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtPrepare2 fail:", error);
		return -1;
	}

	/* 绑定输入参数 */


	ret = OCIStmtExecute(context, stmt, error, 0, 0, (OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtExecute fail:", error);
		return -1;
	}

	/* 输出参数定义 */
	#define ARRAY_SIZE      5

    int         empno[ARRAY_SIZE];
    char        ename[ARRAY_SIZE][11];
    char        job[ARRAY_SIZE][10];
    int         mgr[ARRAY_SIZE];
    double      sal[ARRAY_SIZE];
    double      comm[ARRAY_SIZE];
	double  	deptno[ARRAY_SIZE];

    /* 指示器变量/指示器变量数组，如果此字段可能存在空值，则要指示器变量，否则单条处理时为NULL */
    int         empno_ind[ARRAY_SIZE];
    int         ename_ind[ARRAY_SIZE];
    int         job_ind[ARRAY_SIZE];
    int         mgr_ind[ARRAY_SIZE];
    int         sal_ind[ARRAY_SIZE];
    int         comm_ind[ARRAY_SIZE];
	int			deptno_ind[ARRAY_SIZE];

    OCIDefine   *def1 = NULL;
    OCIDefine   *def2 = NULL;
    OCIDefine   *def3 = NULL;
    OCIDefine   *def4 = NULL;
    OCIDefine   *def5 = NULL;
    OCIDefine   *def6 = NULL;
	OCIDefine	*def7 = NULL;

    /* 字符串 SQLT_STR或SQLT_CHR 整型:SQLT_INT, 浮点: SQLT_FLD */
    OCIDefineByPos(stmt, &def1, error, 1, (void *)&empno[0], (ub4)sizeof(empno[0]), SQLT_INT, (dvoid *)empno_ind, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmt, &def2, error, 2, (void *)ename[0], (ub4)sizeof(ename[0]), SQLT_STR, (dvoid *)ename_ind, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmt, &def3, error, 3, (void *)job[0], (ub4)sizeof(job[0]), SQLT_STR, (dvoid *)job_ind, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmt, &def4, error, 4, (void *)&mgr[0], (ub4)sizeof(mgr[0]), SQLT_INT, (dvoid *)mgr_ind, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmt, &def5, error, 5, (void *)&sal[0], (ub4)sizeof(sal[0]), SQLT_FLT, (dvoid *)sal_ind, NULL, NULL, OCI_DEFAULT);
    OCIDefineByPos(stmt, &def6, error, 6, (void *)&comm[0], (ub4)sizeof(comm[0]), SQLT_FLT, (dvoid *)comm_ind, NULL, NULL, OCI_DEFAULT);
	OCIDefineByPos(stmt, &def7, error, 7, (void *)&deptno[0], (ub4)sizeof(deptno[0]), SQLT_INT, (dvoid *)deptno_ind, NULL, NULL, OCI_DEFAULT);
		
	int done = 1;
    int rows, i;
    while(done)
    {
        ret = OCIStmtFetch(stmt, error, ARRAY_SIZE, OCI_FETCH_NEXT, OCI_DEFAULT);
        if(ret == OCI_SUCCESS || ret == OCI_NO_DATA)
        {
            if(ret == OCI_SUCCESS)
                rows = ARRAY_SIZE;
            if(ret == OCI_NO_DATA)
            {
                ret = OCIAttrGet(stmt, OCI_HTYPE_STMT, &rows, NULL, OCI_ATTR_ROWS_FETCHED, error);
                if(ret != OCI_SUCCESS)
                    PrintError("OCIAttrGet, ", error);
                done = 0;
            }
            printf("fetch rows: %d\n", rows);
            for(i=0; i<rows; i++)
            {
                printf("empno:%d, ename:%s, job:%s, mgr:%d, sal:%.2f, comm:%.2f\n", empno[i], ename[i], job[i], mgr[i], sal[i], comm[i]);
			}
		}
		else
        {
            PrintError("OCIStmtFetch: ", error);
            done = 0;
        }
	}

	OCIStmtRelease(stmt, error, NULL, 0, OCI_DEFAULT);

	return 0;		
}

int update(OCISvcCtx *context, OCIError *error)
{
	OCIStmt     *stmt = NULL;
    OCIBind     *bind1 = NULL;
    OCIBind     *bind2 = NULL;

    int     ret;

    char    *update_sql = "update userinfo set username=:UserName where userid=:UserId";

    ret = OCIStmtPrepare2(context, &stmt, error, (text *)update_sql, strlen(update_sql), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
    if(ret != OCI_SUCCESS)
    {
        PrintError("OCIStmtPrepare2 update, ", error);
        return -1;
    }

    char    username[20] = "Ma XiaoLong";
    char    userid[20] = "201704280003";

    OCIBindByName(stmt, &bind1, error, (text *)":UserName", -1, (void *)username, sizeof(username), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
    OCIBindByName(stmt, &bind2, error, (text *)":UserId", -1, (void *)userid, sizeof(userid), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

    /* OCI_COMMIT_ON_SUCCESS */
    ret = OCIStmtExecute(context, stmt, error, 1, 0, (OCISnapshot *)NULL, (OCISnapshot *)NULL, OCI_COMMIT_ON_SUCCESS);
    if(ret != OCI_SUCCESS)
    {
        PrintError("OCIStmtExecute fail, ", error);
    }
    else
    {
        printf("update success\n");
    }

    OCIStmtRelease(stmt, error, NULL, 0, OCI_DEFAULT);
	
	return 0;
}


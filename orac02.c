/* 使用会话池 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "oci.h"

/* 环境句柄, 所有句柄的基础, 指向一块内存区, 
 * 其他的句柄在该句柄的基础上进行别的句柄空间的分配和操作 */
static OCIEnv		*env = NULL;
static OCIAuthInfo	*auth = NULL;
static OCISPool		*spool = NULL; /* 会话池 */	
static OCICPool		*cpool = NULL; /* 连接池 */

char	username[] = "scott";	 /* 用户名 */
char	password[] = "tiger";	 /* 用户密码 */
char	connstr[] = "";			 /* 服务名称: 192.168.1.46/orcl */

char	*poolName = NULL;
int		 poolNameLen = 0;


#define	DEFAULT_SESSION		10

/* 使用OCISessionGet获取登录数据库权限(推荐)*/
int HandleSQL()
{
	int			ret, i;
	OCIError	*error = NULL;

	ret = OCIHandleAlloc(env, (dvoid **)&error, OCI_HTYPE_ERROR, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc OCI_HTYPE_ERROR error: %d\n", ret);
		goto HandleSQL_end;	
	}

	/* 获取一个会话对象: OCISessionGet 与 OCISessionRelease 两函数想匹配 */
	for(i = 0; i<DEFAULT_SESSION; i++)
	{
		OCISvcCtx   *context = NULL;
		printf("#### Use Session: %d #####\n", i);
		ret = OCISessionGet(env, 
							error, 
							&context, 
							auth, 
							(text *)connstr, 
							strlen(connstr), 
							NULL, 
							0, 
							NULL, 
							NULL, 
							NULL, 
							OCI_DEFAULT);
		if(ret != OCI_SUCCESS)
		{
			PrintError("OCISessionGet context error: ", error);
			goto HandleSQL_end;
		}

		/* 开始SQL语句操作 */
		query_fetch_by_one(context, error);

		query_fetch_by_multi(context, error);

		OCISessionRelease(context, error, NULL, 0, OCI_DEFAULT);
	}

HandleSQL_end:
	if(error)
		OCIHandleFree(error, OCI_HTYPE_ERROR);
}

/* 使用Logon(不推荐) */
int HandleSQL2()
{
	int		ret, i;
	OCIError    *error = NULL;
    
    ret = OCIHandleAlloc(env, (dvoid **)&error, OCI_HTYPE_ERROR, 0, (void **)NULL);
    if(ret != OCI_SUCCESS)
    {   
        printf("OCIHandleAlloc OCI_HTYPE_ERROR error: %d\n", ret);
        goto HandleSQL2_end; 
    }

	for(i=0; i<10; i++)
	{
		OCISvcCtx   *context = NULL;
		ret = OCILogon2(env, 
						error, 
						&context, 
						username, 
						strlen(username), 
						password, 
						strlen(password), 
						poolName, 
						poolNameLen, 		
						OCI_LOGON2_SPOOL);  /* OCI_LOGON2_SPOOL为sessionpool, OCI_LOGON2_CPOOL为connectpool, 
											   OCI_LOGON2_STMTCACHE: 启用语句缓存
											   OCI_LOGON2_PROXY: 代理授权, OCISessionCreate时创建*/
		if(ret)
		{
			PrintError("LogOn Database error: ", error);
			return -1;
		}

		/* 处理数据 SQL */
		query_fetch_by_one(context, error);

		query_fetch_by_multi(context, error);

		OCILogoff(context, error);
	}

HandleSQL2_end:
	if(error)
		OCIHandleFree(error, OCI_HTYPE_ERROR);

	return 0;
}

/* 会话池创建 */
int SessionPoolCreate(OCIError *error)
{
	int         ret;
    ub4         min, max, increment;

    ret = OCIHandleAlloc(env, (dvoid **)&spool, OCI_HTYPE_SPOOL, 0, (void **)NULL);
    if(ret != OCI_SUCCESS)
    {
        printf("OCIHandleAlloc OCI_HTYPE_SPOOL fail\n");
        return -1;
    }

    min = DEFAULT_SESSION;           /*最小会话池数量 */
    max = min + 3;           		 /*最大会话池数量*/
    increment = 1;					 /*自增量*/

    /* 创建回话池 */
	/* OCI_SPC_HOMOGENEOUS: 所有session的用户密码被授权登录 */
    ret = OCISessionPoolCreate( env, 
								error, 
								spool, 
								(text **)&poolName, 
								&poolNameLen, 
								(const text *)connstr, 
								strlen(connstr),
                                min, 
								max, 
								increment, 
								(text *)username, 
								strlen(username), 
								(text *)password, 
								strlen(password), 
								OCI_SPC_HOMOGENEOUS);
    if(ret != OCI_SUCCESS)
    {
        PrintError("OCISessionPoolCreate: ", error);
        return -1;
    }

    printf("PoolName: %s, PoolNameLen:%d, connstr:%s\n", poolName, poolNameLen, connstr);

    return 0;
}

/* 连接池创建 */
int ConnectPoolCreate(OCIError *error)
{
	int			ret;
	ub4         min, max, increment;

	ret = OCIHandleAlloc(env, (dvoid **)&cpool, OCI_HTYPE_CPOOL, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc OCI_HTYPE_CPOOL fail\n");
		return -1;
	}

	ret = OCIConnectionPoolCreate(	env, 
									error, 
									cpool, 
									(text **)&poolName, 
									&poolNameLen,
									(text *)connstr, 
									strlen(connstr),
                   					min, 
									max, 
									increment, 
									(text *)username, 
									strlen(username), 
									(text *)password, 
									strlen(password), 
									OCI_SPC_HOMOGENEOUS );
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIConnectionPoolCreate: ", error);
		return -1;
	}

	printf("PoolName: %s, PoolNameLen:%d, connstr:%s\n", poolName, poolNameLen, connstr);

	return 0;
}

int main()
{
	int		ret;
	OCIError	*error;

	ret = OCIEnvCreate(&env, OCI_DEFAULT, NULL, NULL, NULL, NULL, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIEnvCreate error: %d\n", ret);
		goto end;
	}

	ret = OCIHandleAlloc(env, (dvoid **)&error, OCI_HTYPE_ERROR, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc OCI_HTYPE_ERROR error: %d\n");
		goto end;
	}

	/* 创建会话池 */
	ret = SessionPoolCreate(error);
	if(ret != OCI_SUCCESS)
	{
		printf("create sessionpool fail\n");
		goto end;
	}

	ret = OCIHandleAlloc(env, (dvoid **)&auth, OCI_HTYPE_AUTHINFO, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc OCI_HTYPE_ERROR error: %d\n");
		goto end;
	}

	/* 设置用户名 */
	ret = OCIAttrSet(auth, OCI_HTYPE_AUTHINFO, (void *)username, strlen(username), OCI_ATTR_USERNAME, error);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIAttrSet OCI_ATTR_USERNAME error: ", error);
		goto end;
	}

	/* 设置密码 */
	ret = OCIAttrSet(auth, OCI_HTYPE_AUTHINFO, (void *)password, strlen(password), OCI_ATTR_PASSWORD, error);
	if(ret != OCI_SUCCESS )
	{
		PrintError("OCIAttrSet OCI_ATTR_PASSWORD error: ", error);
		goto end;
	}

	/* 开始处理SQL语句 */
	HandleSQL2();	

	/* 销毁会话池 */
	OCISessionPoolDestroy(spool, error, OCI_DEFAULT);
	
end:
	/* 释放资源 */
	if(auth)
		OCIHandleFree(auth, OCI_HTYPE_AUTHINFO);
	if(error)
		OCIHandleFree(error, OCI_HTYPE_ERROR);
	if(env)
		OCIHandleFree(env, OCI_HTYPE_ENV);
	
	exit(0);
}


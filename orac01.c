/*  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "oci.h"

/* 环境句柄, 所有句柄的基础, 指向一块内存区, 
 * 其他的句柄在该句柄的基础上进行别的句柄空间的分配和操作 */
static OCIEnv		*env = NULL;
static OCIAuthInfo	*auth = NULL;

char	username[] = "scott";
char	password[] = "tiger";
char	connstr[] = "";

#define	DEFAULT_SESSION		10

int HandleSQL()
{
	int			ret;
	OCIError	*error = NULL;
	OCISvcCtx	*context = NULL;

	ret = OCIHandleAlloc(env, (dvoid **)&error, OCI_HTYPE_ERROR, 0, (void **)NULL);
	if(ret != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc OCI_HTYPE_ERROR error: %d\n", ret);
		goto HandleSQL_end;	
	}

	/* 获取一个会话对象 */
	ret = OCISessionGet(env, error, &context, auth, (text *)connstr, strlen(connstr), NULL, 0, NULL, NULL, NULL, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCISessionGet context error: ", error);
		goto HandleSQL_end;
	}

	/* 开始SQL语句操作 */
	query_fetch_by_one(context, error);

	query_fetch_by_multi(context, error);

	OCISessionRelease(context, error, NULL, 0, OCI_DEFAULT);

HandleSQL_end:
	if(error)
		OCIHandleFree(error, OCI_HTYPE_ERROR);
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
	HandleSQL();	
	
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


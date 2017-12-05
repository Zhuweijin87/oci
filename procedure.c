#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oci.h"
#include "util.h"

int call_procedure(OCISvcCtx *context, OCIError *error)
{
	int		ret;
	char	sql[100] = "BEGIN MYPROC3(:uid, :uname, :unation, :ugender);END;";
	
	char		userId[32] = "201704280001";
	char		userName[50] = {'\0'};
	char		nation[20] = {'\0'};
	char		gender[10] = {'\0'};

	OCIStmt		*stmt;

	info("Call Procedure");

	ret = OCIStmtPrepare2(context, &stmt, error, (const text *)sql, strlen(sql), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtPrepare2: ", error);
		return -1;
	}

	OCIBind     *bind1 = NULL;
	OCIBind		*bind2 = NULL;
	OCIBind		*bind3 = NULL;
	OCIBind		*bind4 = NULL;

	OCIBindByPos(stmt, &bind1, error, 1, (void *)userId, sizeof(userId), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmt, &bind1, error, 2, (void *)userName, sizeof(userName), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmt, &bind1, error, 3, (void *)nation, sizeof(nation), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmt, &bind1, error, 4, (void *)gender, sizeof(gender), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);

	ret = OCIStmtExecute(context, stmt, error, 1, 0, (OCISnapshot *) NULL, (OCISnapshot *) NULL, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtExecute: ", error);
		PrintError("SQL: ", sql);
		return -1;
	}

	info("UserName: %s, Nation:%s, Gender: %s", userName, nation, gender);

#if 0
	int 		val_ind;
	OCIDefine	*define1 = NULL;
	
	OCIDefineByPos(stmt, &define1, error, 1, (void *)userName, sizeof(userName), SQLT_STR, (dvoid *)&val_ind, NULL, NULL, OCI_DEFAULT);

	int status = OCIStmtFetch(stmt, error, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
	if(status == OCI_SUCCESS || status == OCI_NO_DATA)
	{
		if(status == OCI_SUCCESS )
        {
        	printf("Output: userName: %s\n", userName);
        }
		else
		{
			printf("No Data Output\n");
		}	

	}
	else
	{
		printf("OCIStmtFetch error\n");
	}
#endif
	return 0;
}

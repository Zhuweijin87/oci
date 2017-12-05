// 插入数据处理

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oci.h"
#include "util.h"

int insert_values(OCISvcCtx *context, OCIError *error)
{
	char	sql[100] = "insert into userinfo(userid, username, birth, nation, gender) values"
						"(:uid, :uname, :birth, :nation, :gender)";

	OCIStmt		*stmt;

	info("Insert Process");

	ret = OCIStmtPrepare2(context, &stmt, error, (text *)update_sql, strlen(update_sql), NULL, 0, OCI_NTV_SYNTAX, OCI_DEFAULT);
	if(ret != OCI_SUCCESS)
	{
		PrintError("OCIStmtPrepare2: ", error);	
		return -1;
	}

	char 		userId[] = "";
	char		userName[] = "";
	char		birth[] = "";
	char		nation[] = "";
	char		gender[] = "";

	OCIBind		*bind1 = NULL;
	OCIBind		*bind2 = NULL;
	OCIBind		*bind3 = NULL;
	OCIBind     *bind4 = NULL;
	OCIBind     *bind5 = NULL;

	OCIBindByPos(stmt, &bind1, error, 1, (void *)userId, sizeof(userId), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);
	OCIBindByPos(stmt, &bind1, error, 2, (void *)userName, sizeof(userName), SQLT_STR, NULL, NULL, NULL, 0, NULL, OCI_DEFAULT);	
	
	return 0;
}


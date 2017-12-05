// 更新数据处理
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oci.h"
#include "util.h"

int update_table_1(OCISvcCtx *context, OCIError *error)
{
	OCIStmt     *stmt = NULL;
    OCIBind     *bind1 = NULL;
    OCIBind     *bind2 = NULL;
    
    int     ret;
    
	info("更新数据操作");

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


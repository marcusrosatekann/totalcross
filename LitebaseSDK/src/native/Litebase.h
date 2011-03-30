/*********************************************************************************
 *  TotalCross Software Development Kit - Litebase                               *
 *  Copyright (C) 2000-2011 SuperWaba Ltda.                                      *
 *  All Rights Reserved                                                          *
 *                                                                               *
 *  This library and virtual machine is distributed in the hope that it will     *
 *  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of    *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                         *
 *                                                                               *
 *********************************************************************************/



/**
 * Declares functions to deal with important Litebase funcionalities.
 */

#ifndef LITEBASE_H
#define LITEBASE_H

#include "tcvm.h"
#include "File.h"
#if defined PALMOS
   #include "palm/File_c.h"
#elif defined WINCE || defined WIN32
   #include "win/File_c.h"
#else
   #include "posix/File_c.h"
#endif

#include "Constants.h"
#include "LitebaseGlobals.h"
#include "Key.h"
#include "Index.h"
#include "LitebaseLex.h"
#include "LitebaseMessage.h"
#include "LitebaseParser.h"
#include "LitebaseTypes.h"
#include "MarkBits.h"
#include "MemoryFile.h"
#include "Node.h"
#include "NormalFile.h"
#include "PlainDB.h"
#include "PreparedStatement.h"
#include "ResultSet.h"
#include "SQLBooleanClause.h"
#include "SQLBooleanClauseTree.h"
#include "SQLColumnListClause.h"
#include "SQLDeleteStatement.h"
#include "SQLInsertStatement.h"
#include "SQLSelectStatement.h"
#include "SQLUpdateStatement.h"
#include "SQLValue.h"
#include "Table.h"
#include "TCVMLib.h"
#include "UtilsLB.h"
#include "Value.h"

/**
 * Loads the necessary data when using Litebase for the first time.
 *
 * @param params Some parameters and function pointers in order to load a .dll.
 * @return <code>false</code> if an error occurs; <code>true</code>, otherwise.
 */
LB_API bool LibOpen(OpenParams params);

/**
 * Flushs all pending data and destroy all Litebase structures when closing the application.
 */
LB_API void LibClose();

/**
 * Loads the necessary data when using Litebase for the first time.
 *
 * @param params Some parameters and function pointers in order to load a .dll.
 * @return <code>false</code> if an error occurs; <code>true</code>, otherwise.
 * @throws OutOfMemoryError if a memory allocation fails.
 */
bool initVars(OpenParams params);

/**
 * Creates a LitebaseConnection for the given creator id and with the given connection param list. This method avoids the creation of more than
 * one instance with the same creator id and parameters, which would lead to performance and memory problems.
 *
 * @param context The thread context where the function is being executed.
 * @param crid The creator id, which may be the same one of the current application and MUST be 4 characters long.
 * @param objParams Only the folder where it is desired to store the tables, <code>null</code>, if it is desired to use the current data 
 * path, or <code>chars_type = chars_format; path = source_path</code>, where <code>chars_format</code> can be <code>ascii</code> or 
 * <code>unicode</code>, and <code>source_path</code> is the folder where the tables will be stored. The params can be entered in any order. If
 * only the path is passed as a parameter, unicode is used. Notice that path must be absolute, not relative.
 * <p>If it is desired to store the database in the memory card (on Palm OS devices only), use the desired volume in the path given to the method.
 * <p>Most PDAs will only have one card, but others, like Tungsten T5, can have more then one. So it is necessary to specify the desired card 
 * slot.
 * <p>Note that databases belonging to multiple applications can be stored in the same path, since all tables are prefixed by the application's 
 * creator id.
 * <p>Also notice that to store Litebase files on card on Pocket PC, just set the second parameter to the correct directory path.
 * <p>It is not recommended to create the databases directly on the PDA. Memory cards are FIVE TIMES SLOWER than the main memory, so it will take
 * a long time to create the tables. Even if the NVFS volume is used, it can be very slow. It is better to create the tables on the desktop, and 
 * copy everything to the memory card or to the NVFS volume.
 * <p>Due to the slowness of a memory card and the NVFS volume, all queries will be stored in the main memory; only tables and indexes will be 
 * stored on the card or on the NVFS volume.
 * <p> An exception will be raised if tables created with an ascii kind of connection are oppened with an unicode connection and vice-versa.
 * @return A Litebase instance.
 * @throws OutOfMemoryError If memory allocation fails.
 */
Object create(Context context, int32 crid, Object objParams);

/**
 * Frees all data concerning a certaim driver connection.
 *
 * @param context The thread context where the function is being executed.
 * @param driver The driver as an int because this function may be called from a hash table.
 */
void freeLitebase(Context context, int32 driver);

/**
 * Used to execute a <code>create table</code> or <code>create index</code> SQL commands.
 * 
 * <p>Examples:
 * <ul>
 *     <li><code>driver.execute("create table PERSON (NAME CHAR(30), SALARY DOUBLE, AGE INT, EMAIL CHAR(50))");</code>
 *     <li><code>driver.execute("CREATE INDEX IDX_NAME ON PERSON(NAME)");</code>
 * </ul>
 * 
 * <p>When creating an index, its name is ignored but must be given. The index can be created after data was added to the table.
 *
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param sqlStr The SQL creation command.
 * @param sqlLen The SQL string length.
 * @throws SQLParseException If the table name or a default string is too big, there is an invalid default value, or an unknown (on a create table)
 * or repeated column name, or an invalid number occurs.
 * @throws AlreadyCreatedException If the table or index is already created.
 * @throws OutOfMemoryError If a memory allocation fails.
 */
void litebaseExecute(Context context, Object driver, JCharP sqlStr, uint32 len);

/**
 * Used to execute updates in a table (insert, delete, update, alter table, drop). E.g.:
 *
 * <p><code>driver.executeUpdate(&quot;drop table person&quot;);</code> will drop also the indices.
 * <p><code>driver.executeUpdate(&quot;drop index * on person&quot;);</code> will drop all indices but not the primary key index.
 * <p><code>driver.executeUpdate(&quot;drop index name on person&quot;);</code> will drop the index for the &quot;name&quot; column.
 * <p><code> driver.executeUpdate(&quot;ALTER TABLE person DROP primary key&quot;);</code> will drop the primary key.
 * <p><code>driver.executeUpdate(&quot;update person set age=44, salary=3200.5 where name = 'guilherme campos hazan'&quot;);</code> 
 * will update the table.
 * <p><code>driver.executeUpdate(&quot;delete person where name like 'g%'&quot;);</code> will delete records of the table.
 * <p><code> driver.executeUpdate(&quot;insert into person (age, salary, name, email)
 *  values (32, 2000, 'guilherme campos hazan', 'guich@superwaba.com.br')&quot;);</code> will insert a record in the table.
 *
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param sqlStr The SQL creation command.
 * @param sqlLen The SQL string length.
 * @return The number of rows affected or <code>0</code> if a drop or alter operation was successful, or <code>-1</code> if an error occurs.
 * @throws OutOfMemoryError If a memory allocation fails.
 */
int32 litebaseExecuteUpdate(Context context, Object driver, JCharP sqlStr, int32 sqlLen);

/**
 * Used to execute queries in a table. Example:
 * 
 * <pre>
 * ResultSet rs = driver.executeQuery(&quot;select rowid, name, salary, age from person where age != 44&quot;);
 * rs.afterLast();
 * while (rs.prev())
 *    Vm.debug(rs.getString(1) + &quot;. &quot; + rs.getString(2) + &quot; - &quot; + rs.getInt(&quot;age&quot;) + &quot; years&quot;);
 * </pre>
 * 
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param sqlStr The SQL creation command.
 * @param sqlLen The SQL string length.
 * @return A result set with the values returned from the query or <code>null</code> if an error occurs.
 * @throws OutOfMemoryError If a memory allocation fails.
 */
Object litebaseExecuteQuery(Context context, Object driver, JCharP strSql, int32 length);

/**
 * Drops a table.
 * 
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param parser The parser.
 * @throws DriverException If the table does not exist, if its name is greater than the maximum possible or it is not possible to remove it.
 */
void litebaseExecuteDropTable(Context context, Object driver, LitebaseParser* parser);

/**
 * Drops an index.
 * 
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param parser The parser.
 * @return <code>-1</code> if an error occured; the number of indices removed, otherwise.
 * @throws DriverException If a column does not have an index, is invalid, or if the columns to have the index dropped are from a primary key.
 */
int32 litebaseExecuteDropIndex(Context context, Object driver, LitebaseParser* parser);

/**
 * Executes an alter statement.
 *
 * @param context The thread context where the function is being executed.
 * @param driver The current Litebase connection.
 * @param parser The parser.
 * @throws DriverException If there is no primary key to be dropped, or an invalid column name.
 * @throws AlreadyCreatedException If one tries to add another primary key.
 * @throws SQLParseException If there is a blob in a primary key definition or there is a duplicated column name in the primary key definition.
 * @throws OutOfMemoryError If a memory allocation fails.
 */
void litebaseExecuteAlter(Context context, Object driver, LitebaseParser* parser);

/**
 * Gets the value of a column stored in the row iterator.
 *
 * @param p->obj[0] The row iterator.
 * @param p->i32[0] The column index.
 * @param type The type of the column.
 * @param p->retI Receives an int or a short.
 * @param p->retL Receives a long.
 * @param p->retD Receives a float or a double.
 * @param p->retO Receives a string, blob, date, or datetime.
 * @throws NullPointerException If the row iterator is closed (table is null).
 */
void getByIndex(NMParams p, int32 type);

/** 
 * Checks if the path passed as a parameter is valid and uses an internal path if it is null.
 *
 * @param context The thread context where the function is being executed.
 * @param sourcePath Receives the path that Litebase will use to store and access tables.
 * @param pathParam the path passed as a parameter.
 * @return The slot number for palm, -1 for the other devices or 0 in case of error.
 * @throws DriverException if the path passed as a parameter is invalid. 
 */
int32 checkApppath(Context context, CharP sourcePath, CharP pathParam);

/**
 * Verifies if the function can be applied to a data type field.
 * 
 * @param parameterDataType The data type of the function parameter.
 * @param sqlFunction The function code.
 * @return <code>true</code> If the function can be applied to a data type field; <code>false</code>, otherwise.
 */
bool bindFunctionDataType(int32 parameterDataType, int32 sqlFunction); 

/**
 * Returns the name of the data type function.
 *
 * @param sqlFunction The function code.
 * @return A string with the function name. 
 */
CharP dataTypeFunctionsName(int32 sqlFunction); 

#ifdef ENABLE_TEST_SUITE

/**
 * Tests if <code>LibClose()</code> finished some structures.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_LibClose(TestSuite* testSuite, Context currentContext);  

/**
 * Tests if <code>LibOpen()</code> initializes all the needed variables.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_LibOpen(TestSuite* testSuite, Context currentContext);   

/**
 * Tests the function <code>bindFunctionDataType()</code> works properly. It is tested with all possible data types and data function types, including 
 * invalid values.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_bindFunctionDataType(TestSuite* testSuite, Context currentContext);

/**
 * Tests if the function <code>checkApppath()</code> accepts paths if and only if they are valid.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_checkApppath(TestSuite* testSuite, Context currentContext);

/**
 * Tests if the function <code>dataTypeFunctionsName()</code> returns the strings with the correct types.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_dataTypeFunctionsName(TestSuite* testSuite, Context currentContext);

/**
 * Tests if <code>initVars()</code> initializes all the needed variables.
 * 
 * @param testSuite The test structure.
 * @param currentContext The thread context where the test is being executed.
 */
void test_initVars(TestSuite* testSuite, Context currentContext);  

#endif

#endif 

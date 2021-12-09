USE GHNDDB
IF EXISTS (SELECT 1
            FROM  sysobjects
           WHERE  id = object_id('Department')
            AND   type = 'U')
   DROP TABLE [Department]
GO
IF EXISTS (SELECT 1
            FROM  sysobjects
           WHERE  id = object_id('Employee')
            AND   type = 'U')
   DROP TABLE [Employee]
GO
CREATE TABLE Department
(
   orgIndexCode		NVARCHAR(100)        NOT NULL ,
   orgName			NVARCHAR(60)		 NULL ,
   orgEmployeesNumber INT,
   islocal			INT,		-- 是否是本地部门，0-不是，1-是
   isModify			INT,		-- 是否修改过，0-不是，1-是
   orgNo			NVARCHAR(60),
   orgPath			NVARCHAR(60),
   CreationTime     DATETIME             NOT NULL DEFAULT GETDATE(),
   LastModify       DATETIME             NULL,
   DepartDesc		NVARCHAR(255)		 NULL ,
)
GO
CREATE TABLE Employee
(	
	personId			NVARCHAR(60) NOT NULL,
	personName			NVARCHAR(60),
	pinyin				NVARCHAR(40),
	gender				INT,
	phoneNo				NVARCHAR(20),
	jobNo				NVARCHAR(20),
	orgIndexCode		NVARCHAR(100) NOT NULL,
	orgName				NVARCHAR(60),
	certificateNo		NVARCHAR(60),
	certificateType		INT,
	creationTime		DATETIME             NOT NULL DEFAULT GETDATE(),
	lastModify			DATETIME             NULL,
	personDesc			NVARCHAR(255)		 NULL 
)
GO

USE [PBAccount]
GO

/****** Object:  Table [dbo].[Account]    Script Date: 2014-08-19 ¿ÀÀü 9:20:54 ******/
SET ANSI_NULLS ON
GO

SET QUOTED_IDENTIFIER ON
GO

CREATE TABLE [dbo].[Account](
	[AccountID] [nvarchar](60) NOT NULL,
	[Passwd] [nvarchar](24) COLLATE SQL_Latin1_General_CP1_CS_AS NOT NULL,
	[GameCash] [int] NOT NULL,
	[UpdateDate] [smalldatetime] NOT NULL,
	[CreateDate] [smalldatetime] NOT NULL,
	[Active] [bit] NOT NULL,
 CONSTRAINT [PKC_Account_AccountID] PRIMARY KEY CLUSTERED 
(
	[AccountID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON) ON [PRIMARY]
) ON [PRIMARY]

GO



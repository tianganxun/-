<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

   rs.open "select * from zuoye",conn,0,2

      rs.addnew
      rs("作业序号")=request.form("xh")
      rs("作业科目")=request.form("xm")
      rs("作业概述")=request.form("xb")
      rs("banji")=request.form("banji")
      rs("截止日期")=request.form("jdate")
      rs.update
        
     response.redirect "zuoyebuzhi.htm"
      
     rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing




%>
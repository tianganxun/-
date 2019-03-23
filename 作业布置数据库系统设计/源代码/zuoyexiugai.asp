<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

       rs.open "select * from zuoye where trim(作业序号)='" & request.form("xh") & "'",conn,,3

      
   
    
      rs("作业科目")=request.form("xm")
      rs("作业概述")=request.form("xb")
      rs("banji")=request.form("banji")
      rs("截止日期")=request.form("jdate")
      rs.update
        
     
      
     rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing
response.redirect "zuoyexiugai.htm"



%>
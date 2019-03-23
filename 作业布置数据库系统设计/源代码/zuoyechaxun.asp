<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

  rs.open "select * from zuoye where trim(banji)='"&request.form("banji")&"'",conn

  response.write "<table border=1 align='center'>"
  response.write "<tr><th>作业序号</th><th>作业科目</th><th>作业概述</th>" &_
                "<th>班级</th><th>截止日期</th></tr>"
do while not rs.eof
    response.write "<tr>"
    response.write "<td>"&rs("作业序号")&"</td>"
    response.write "<td>"&rs("作业科目")&"</td>"
    response.write "<td>"&rs("作业概述")&"</td>"
    response.write "<td>"&rs("banji")&"</td>"
    response.write "<td>"&rs("截止日期")&"</td>"
    response.write "</tr>"
    rs.movenext
    loop

    response.write "</table>"
   
    response.write "<p align='center'>"
    response.write "<a href='zuoyechaxun.htm'>返回查询界面</a></p>"

    rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing

        
    
      
   




%>
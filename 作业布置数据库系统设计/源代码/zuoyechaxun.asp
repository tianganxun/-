<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

  rs.open "select * from zuoye where trim(banji)='"&request.form("banji")&"'",conn

  response.write "<table border=1 align='center'>"
  response.write "<tr><th>��ҵ���</th><th>��ҵ��Ŀ</th><th>��ҵ����</th>" &_
                "<th>�༶</th><th>��ֹ����</th></tr>"
do while not rs.eof
    response.write "<tr>"
    response.write "<td>"&rs("��ҵ���")&"</td>"
    response.write "<td>"&rs("��ҵ��Ŀ")&"</td>"
    response.write "<td>"&rs("��ҵ����")&"</td>"
    response.write "<td>"&rs("banji")&"</td>"
    response.write "<td>"&rs("��ֹ����")&"</td>"
    response.write "</tr>"
    rs.movenext
    loop

    response.write "</table>"
   
    response.write "<p align='center'>"
    response.write "<a href='zuoyechaxun.htm'>���ز�ѯ����</a></p>"

    rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing

        
    
      
   




%>
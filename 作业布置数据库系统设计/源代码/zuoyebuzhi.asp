<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

   rs.open "select * from zuoye",conn,0,2

      rs.addnew
      rs("��ҵ���")=request.form("xh")
      rs("��ҵ��Ŀ")=request.form("xm")
      rs("��ҵ����")=request.form("xb")
      rs("banji")=request.form("banji")
      rs("��ֹ����")=request.form("jdate")
      rs.update
        
     response.redirect "zuoyebuzhi.htm"
      
     rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing




%>
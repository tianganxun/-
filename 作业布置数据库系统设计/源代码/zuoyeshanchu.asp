<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

   rs.open "select * from zuoye where ��ҵ���='" & request.form("xh") & "'",conn,0,2

      rs.delete
        
     response.redirect "zuoyeshanchu.htm"
      
     rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing




%>
<%

  
    set conn=server.createobject("adodb.connection")
    conn.open "shujuku"
    
    set rs=server.createobject("adodb.recordset")

       rs.open "select * from zuoye where trim(��ҵ���)='" & request.form("xh") & "'",conn,,3

      
   
    
      rs("��ҵ��Ŀ")=request.form("xm")
      rs("��ҵ����")=request.form("xb")
      rs("banji")=request.form("banji")
      rs("��ֹ����")=request.form("jdate")
      rs.update
        
     
      
     rs.close
    Set rs=Nothing
    conn.close
    Set conn=nothing
response.redirect "zuoyexiugai.htm"



%>
<%

Set conn = Server.CreateObject("adodb.connection")
conn.open "shujuku"

Set rs = Server.CreateObject("adodb.recordset")
sqlc="select * from login1 where username='"&request.Form("username")&"' and pwd='"&request.Form("password")&"'"
rs.open sqlc,conn,0,2

     if  rs.eof and rs.eof then
	Response.redirect "denglu.htm"
        else
         if rs("leixing")=1 then
         Response.redirect "jiaoshiyemian.htm"
         else
         Response.redirect "zuoyechaxun.htm"
end if
end if
rs.Close
Set rs  = Nothing


conn.Close
Set conn = Nothing %>
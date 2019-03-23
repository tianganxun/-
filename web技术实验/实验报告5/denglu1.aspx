<%@ Page Language="C#" AutoEventWireup="true" CodeFile="denglu1.aspx.cs" Inherits="denglu1" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title></title>
</head>
<body>
    <form runat="server">

<div><b>用户登录</b></div>

          <div>  <label id="label1" for="username">用户名：</label>
            <asp:TextBox ID="username" runat="server"></asp:TextBox></div>
        
           <div>  <label id="label3" for="username">性别：</label>
            <asp:TextBox ID="TextBox1" runat="server"></asp:TextBox></div>
          <div>  <label id="label2" for="psd">密码：</label>
            <asp:TextBox ID="psd" runat="server"></asp:TextBox></div>

         
      
          <div>  <asp:Button ID="Button1" runat="server" Text="登录" onclick="Denglu_Click"/></div>


  


</form>
</body>
</html>

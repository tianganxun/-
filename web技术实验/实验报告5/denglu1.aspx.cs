using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;


using System.Windows.Forms;




public partial class denglu1 : System.Web.UI.Page
{
    protected void Page_Load(object sender, EventArgs e)
    {

        
    }
   
        
          
    public void Denglu_Click(object sender, EventArgs e)
    {
        string name = username.Text;

        if (name == "tgx")
        {
            if (Request.Cookies["tgx"] == null)
            {

                Response.Cookies["tgx"]["c"] = "1";
                Response.Cookies["tgx"].Expires = DateTime.MaxValue;
                String currenttime = DateTime.Now.ToString();


                MessageBox.Show("这是你第1次访问该页面");

            }
            else
            {
                int vNumbe = 0;
                string a = Request.Cookies["tgx"]["c"];
                vNumbe = Convert.ToInt32(a) + 1;
                a = Convert.ToString(vNumbe);
                Response.Cookies["tgx"]["c"] = a;
                MessageBox.Show("这是你第" + a + "次访问该页面");
            }
        }
    }

   



}




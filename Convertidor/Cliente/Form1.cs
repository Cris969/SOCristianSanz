using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text;

namespace Convertidor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBox1.Text == "")
            {
                MessageBox.Show("Introduce un valor");
            }
            else
            {
                //Creamos el IPEndPoint con el ip y el puerto del servidor
                IPAddress direc = IPAddress.Parse("192.168.56.104");
                IPEndPoint ipep = new IPEndPoint(direc, 9000);
                //Creamos el socket
                Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);
                }
                catch (SocketException ex)
                {
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
                string mensaje = "1/" + textBox1.Text;
                //Enviamos al servidor la peticion del usuario
                //Enviamos al servidor un vector de bytes
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);
                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                MessageBox.Show("La temperatura en Celcius es " + mensaje);
                server.Shutdown(SocketShutdown.Both);
                server.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (textBox1.Text == "")
            {
                MessageBox.Show("Introduce un valor");
            }
            else
            {
                //Creamos el IPEndPoint con el ip y el puerto del servidor
                IPAddress direc = IPAddress.Parse("192.168.56.104");
                IPEndPoint ipep = new IPEndPoint(direc, 9000);
                //Creamos el socket
                Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                try
                {
                    server.Connect(ipep);
                }
                catch (SocketException ex)
                {
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
                string mensaje = "2/" + textBox1.Text;
                //Enviamos al servidor la peticion del usuario
                //Enviamos al servidor un vector de bytes
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);
                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                MessageBox.Show("La temperatura en Fahrenheit es " + mensaje);
                server.Shutdown(SocketShutdown.Both);
                server.Close();
            }
        }
    }
}

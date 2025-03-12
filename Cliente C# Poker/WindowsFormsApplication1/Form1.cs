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

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        Socket server;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
            //al que deseamos conectarnos
            IPAddress direc = IPAddress.Parse(IP.Text);
            IPEndPoint ipep = new IPEndPoint(direc, 9014);
            

            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.BackColor = Color.Green;
                MessageBox.Show("Conectado");

            }
            catch (SocketException ex)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            }

        }

        // Botón para registrar
        private void buttonRegister_Click_1(object sender, EventArgs e)
        { 
            //enviar
            string mensaje = "REGISTER/" + nombre.Text + "/" + cuenta.Text + "/" + contraseña.Text;
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //repsuesta
            byte[] msg2 = new byte[512];
            int bytesRecibidos = server.Receive(msg2);
            string respuesta = Encoding.ASCII.GetString(msg2, 0, bytesRecibidos).Trim('\0');  // Limpiar la respuesta

            if (respuesta == "1/REGISTERED")
                MessageBox.Show("Registro exitoso.");
            else
                MessageBox.Show("Error en el registro.");
        }

        // Botón para iniciar sesión
        private void buttonLogin_Click_1(object sender, EventArgs e)
        {
            //enviar
            string mensaje = "LOGIN/" + cuenta.Text + "/" + contraseña.Text;
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            //respuesta
            byte[] msg2 = new byte[512];
            int bytesRecibidos = server.Receive(msg2);
            string respuesta = Encoding.ASCII.GetString(msg2, 0, bytesRecibidos).Trim('\0');  // Limpiar la respuesta

            if (respuesta == "2/LOGGED_IN")
                MessageBox.Show("Inicio de sesión exitoso.");
            else if (respuesta == "ERROR AL INSERTAR EL NUEVO USUARIO")
                MessageBox.Show("ERROR AL INSERTAR EL NUEVO USUARIO");
            else if (respuesta == "ERROR USUARIO CON LA MISMA CUENTA")
                MessageBox.Show("ERROR USUARIO CON LA MISMA CUENTA");
            else                        MessageBox.Show("Error en el inicio de sesión.");
        }

        private void Desconectar_Click(object sender, EventArgs e)
        {
            string mensaje = "0/";

            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            // Nos desconectamos
            this.BackColor = Color.Gray;
            server.Shutdown(SocketShutdown.Both);
            server.Close();
        }

    }
}

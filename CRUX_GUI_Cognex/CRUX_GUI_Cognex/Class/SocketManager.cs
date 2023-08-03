using System;
using System.IO;
using System.Net.Sockets;
using System.Runtime.Serialization.Formatters.Binary;
using BinaryFormatterStructs_DLL;
using System.Net;
using System.Threading;
using System.Drawing;

namespace CRUX_GUI_Cognex.Class
{
    // TCP 통신 클래스 작성 중 YWS
    public class SocketManager
    {

    }

    public class SocketClient
    {
        public TcpClient client;
        public NetworkStream networkStream;

        public string strServerIP;
        public int nServerPort;

        public SocketClient(string serverIP, int serverPort)
        {
            strServerIP = serverIP;
            nServerPort = serverPort;
        }

        public void Connect()
        {
            while (true)
            {
                try
                {
                    client = new TcpClient(strServerIP, nServerPort);
                    break;
                }
                catch (SocketException)
                {
                    // 0.3초 슬립 후, 다시 시작해본다.
                    Thread.Sleep(300);
                }
            }

            networkStream = client.GetStream();
        }

        // 클라이언트 연결 끊어져도 자동으로 재연결 실행하도록 수정
        public void ClientConnect()
        {
            while (true)
            {
                // 최초 연결 이라면
                if (client == null)
                {
                    Connect();
                }
                // 연결 끊어짐
                else if(!IsCanWrite())
                {
                    Close();
                    Connect();
                }
                // 연결 유지중
                else if(IsCanWrite())
                {
                    Thread.Sleep(1000); // 1초 슬립
                }
            }
        }

        // empty Data로 연결상태 확인
        public bool IsCanWrite()
        {
            try
            {
                byte[] empty = { 0 };
                networkStream.Write(empty, 0, 0);
            }
            catch
            {
                return false;
            }

            return true;
        }

        public void Start()
        {
            Thread t = new Thread(new ThreadStart(ClientConnect));
            t.Start();
        }

        // 1. byte[], 보낼 data, 2. offset, byte[]에서 보낼 시작 위치, 3. length, 보낼 데이터 길이
        public bool Send(byte[] data, int offset, int length)
        {
            try
            {
                networkStream.Write(data, offset, length);
            }
            catch
            {
                // 오류 발생
                Close();
                return false;
            }

            // 정상 종료
            return true;
        }

        public bool Send(TcpDataInfo data)
        {
            try
            {
                byte[] byteAry;
                TcpDataInfo tempinfo = new TcpDataInfo();

                // Struct to byte[]
                BinaryFormatter formatter = new BinaryFormatter();
                using (MemoryStream memoryStream = new MemoryStream())
                {
                    formatter.Serialize(memoryStream, data);
                    byteAry = memoryStream.ToArray();
                }

                networkStream.Write(byteAry, 0, byteAry.Length);
            }
            catch
            {
                // 오류 발생
                Close();
                return false;
            }

            // 정상 종료
            return true;
        }

        public void Close()
        {
            if (networkStream != null)
            {
                networkStream.Close();
                networkStream = null;
            }

            if (client != null)
            {
                client.Close();
                client = null;
            }
        }
    }

    public class SocketServer : SocketClient
    {
        public TcpListener listener;
        byte[] data = new byte[1024 * 1024 * 1024]; // 1GB

        public SocketServer(string serverIP, int serverPort) : base(serverIP, serverPort)
        {
        }

        public void Listen()
        {
            listener = new TcpListener(IPAddress.Any, nServerPort);
            listener.Start();
            client = listener.AcceptTcpClient();
            networkStream = client.GetStream();
        }

        public void ServerConnect()
        {
            while (true)
            {
                try
                {
                    // 최초 연결
                    if (client == null)
                    {
                        Listen();
                    }
                    else
                    {
                        int receivedDataSize = 0;
                        receivedDataSize = networkStream.Read(data, 0, data.Length);

                        // Data Size가 0이면, Client와 연결 끊김... 재연결 필요 예외처리
                        if (receivedDataSize == 0)
                        {
                            Close();
                        }
                        else
                        {
                            // 정상 Data일 경우 구조체 파싱.
                            ReadData(receivedDataSize);
                        }
                    }
                }
                catch
                {
                    // Server Start 못함. 로그 추가 필요
                    Close();
                    break;
                }
            }
        }

        new public void Start()
        {
            Thread t = new Thread(new ThreadStart(ServerConnect));
            t.Start();
        }

        public void ReadData(int recvDataSize)
        {
            MemoryStream memoryStream;
            Bitmap bitmap;

            byte[] byteAry = new byte[recvDataSize];
            Array.Copy(data, 0, byteAry, 0, recvDataSize);

            TcpDataInfo tempinfo = new TcpDataInfo();

            BinaryFormatter formatter = new BinaryFormatter();
            using (MemoryStream memBuffer = new MemoryStream(byteAry))
            {
                tempinfo = (TcpDataInfo)formatter.Deserialize(memBuffer);
            }

            // test code... image만 추출해서 bitmap으로 저장
            memoryStream = new MemoryStream(tempinfo.byteImageBuffer, 0, tempinfo.byteImageBuffer.Length);
            bitmap = new Bitmap(memoryStream);
            bitmap.Save("D:\\Result.bmp");

            memoryStream.Dispose();
            memoryStream = null;
        }

        new public void Close()
        {
            base.Close();

            if (listener != null)
            {
                listener.Stop();
                listener = null;
            }
        }
    }
}

﻿using PluginInterface;
using S7.Net;
using System;
using System.Text;

namespace DriverSiemensS7
{
    [DriverSupported("1500")]
    [DriverSupported("1200")]
    [DriverSupported("400")]
    [DriverSupported("300")]
    [DriverSupported("200")]
    [DriverSupported("200Smart")]
    [DriverInfoAttribute("SiemensS7", "V1.0.0", "Copyright WHD© 2021-12-19")]
    public class SiemensS7 : IDriver
    {
        private Plc plc = null;
        #region 配置参数

        [ConfigParameter("设备Id")]
        public Guid DeviceId { get; set; }

        [ConfigParameter("PLC类型")]
        public CpuType CpuType { get; set; } = CpuType.S71200;

        [ConfigParameter("IP地址")]
        public string IpAddress { get; set; } = "127.0.0.1";

        [ConfigParameter("端口号")]
        public int Port { get; set; } = 102;

        [ConfigParameter("Rack")]
        public short Rack { get; set; } = 0;

        [ConfigParameter("Slot")]
        public short Slot { get; set; } = 0;

        [ConfigParameter("超时时间ms")]
        public int Timeout { get; set; } = 3000;

        [ConfigParameter("最小通讯周期ms")]
        public uint MinPeriod { get; set; } = 3000;

        #endregion

        public SiemensS7(Guid deviceId)
        {
            DeviceId = deviceId;
        }


        public bool IsConnected
        {
            get
            {
                return plc != null && plc.IsConnected;
            }
        }

        public bool Connect()
        {
            try
            {
                plc = new Plc(CpuType, IpAddress, Port, Rack, Slot);
                plc.ReadTimeout = Timeout;
                plc.WriteTimeout = Timeout;
                plc.Open();
            }
            catch (Exception)
            {
                return false;
            }
            return IsConnected;
        }

        public bool Close()
        {
            try
            {
                plc?.Close();
                return !IsConnected;
            }
            catch (Exception)
            {

                return false;
            }
        }

        public void Dispose()
        {
            try
            {
                plc = null;
            }
            catch (Exception)
            {

            }
        }

        [Method("读西门子PLC标准地址", description: "读西门子PLC标准地址")]
        public DriverReturnValueModel Read(DriverAddressIoArgModel ioarg)
        {
            var ret = new DriverReturnValueModel { StatusType = VaribaleStatusTypeEnum.Good };

            if (plc != null && plc.IsConnected)
            {
                try
                {
                    ret.Value = plc.Read(ioarg.Address);
                    if (ioarg.ValueType == DataTypeEnum.Float)
                    {
                        var buffer = new byte[4];

                        buffer[3] = (byte)((uint)ret.Value >> 24);
                        buffer[2] = (byte)((uint)ret.Value >> 16);
                        buffer[1] = (byte)((uint)ret.Value >> 8);
                        buffer[0] = (byte)((uint)ret.Value >> 0);
                        ret.Value = BitConverter.ToSingle(buffer, 0);
                    }
                }
                catch (Exception ex)
                {

                    ret.StatusType = VaribaleStatusTypeEnum.Bad;
                    ret.Message = $"读取失败,{ex.Message}";
                }
            }
            else
            {
                ret.StatusType = VaribaleStatusTypeEnum.Bad;
                ret.Message = "连接失败";
            }
            return ret;
        }

        [Method("读字符串", description: "读字符串")]
        public DriverReturnValueModel ReadString(DriverAddressIoArgModel ioarg)
        {
            var ret = new DriverReturnValueModel { StatusType = VaribaleStatusTypeEnum.Good };

            if (plc != null && plc.IsConnected)
            {
                try
                {
                    int db = int.Parse(ioarg.Address.Trim().Split(',')[0]);
                    int startAdr = int.Parse(ioarg.Address.Trim().Split(',')[1]);
                    int count = int.Parse(ioarg.Address.Trim().Split(',')[2]);
                    var buffers = plc.ReadBytes(DataType.DataBlock, db, startAdr, count);
                    ret.Value = Encoding.ASCII.GetString(buffers);
                }
                catch (Exception ex)
                {

                    ret.StatusType = VaribaleStatusTypeEnum.Bad;
                    ret.Message = $"读取失败,{ex.Message}";
                }
            }
            else
            {
                ret.StatusType = VaribaleStatusTypeEnum.Bad;
                ret.Message = "连接失败";
            }
            return ret;
        }

        //预留了大小端转换的 
        private ushort[] ChangeBuffersOrder(ushort[] buffers, DataTypeEnum dataType)
        {
            var newBuffers = new ushort[buffers.Length];
            if (dataType.ToString().Contains("32") || dataType.ToString().Contains("Float"))
            {
                var A = buffers[0] & 0xff00;//A
                var B = buffers[0] & 0x00ff;//B
                var C = buffers[1] & 0xff00;//C
                var D = buffers[1] & 0x00ff;//D
                if (dataType.ToString().Contains("_1"))
                {
                    newBuffers[0] = (ushort)(A + B);//AB
                    newBuffers[1] = (ushort)(C + D);//CD
                }
                else if (dataType.ToString().Contains("_2"))
                {
                    newBuffers[0] = (ushort)((A >> 8) + (B << 8));//BA
                    newBuffers[1] = (ushort)((C >> 8) + (D << 8));//DC
                }
                else if (dataType.ToString().Contains("_3"))
                {
                    newBuffers[0] = (ushort)((C >> 8) + (D << 8));//DC
                    newBuffers[1] = (ushort)((A >> 8) + (B << 8));//BA
                }
                else
                {
                    newBuffers[0] = (ushort)(C + D);//CD
                    newBuffers[1] = (ushort)(A + B);//AB
                }
            }
            else if (dataType.ToString().Contains("64") || dataType.ToString().Contains("Double"))
            {
                if (dataType.ToString().Contains("_1"))
                {

                }
                else
                {
                    newBuffers[0] = buffers[3];
                    newBuffers[1] = buffers[2];
                    newBuffers[2] = buffers[1];
                    newBuffers[3] = buffers[0];
                }
            }
            else
            {
                if (dataType.ToString().Contains("_1"))
                {
                    var h8 = buffers[0] & 0xf0;
                    var l8 = buffers[0] & 0x0f;
                    newBuffers[0] = (ushort)(h8 >> 8 + l8 << 8);
                }
                else
                    newBuffers[0] = buffers[0];
            }
            return newBuffers;
        }

        public async Task<RpcResponse> WriteAsync(string RequestId, string Method, DriverAddressIoArgModel Ioarg)
        {
            RpcResponse rpcResponse = new() { IsSuccess = false, Description = "设备驱动内未实现写入功能" };
            return rpcResponse;
        }
    }
}
namespace Mozi.Encode.CBOR
{
    //�����ͳһ��ȡUTF-8����
    /// <summary>
    /// �ַ�������
    /// </summary>
    public static class StringEncoder
    {
        /// <summary>
        /// ����
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static byte[] Encode(string data)
        {
            return System.Text.Encoding.UTF8.GetBytes(data);
        }
        /// <summary>
        /// ����
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static string Decode(byte[] data)
        {
            return Decode(data, 0, data.Length);
        }
        /// <summary>
        /// ����
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static string Decode(byte[] data, int index, int count)
        {
            return System.Text.Encoding.UTF8.GetString(data, index, count);
        }
    }
}
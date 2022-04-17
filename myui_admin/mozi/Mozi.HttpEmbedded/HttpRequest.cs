using System;
using System.Collections.Generic;
using System.Text;
using Mozi.HttpEmbedded.Cookie;
using Mozi.HttpEmbedded.Encode;
using Mozi.HttpEmbedded.Generic;

namespace Mozi.HttpEmbedded
{
    //DONE Ӧ�� GET/POST ��ѯ�ֶν������� 

    /// <summary>
    /// HTTP����
    /// </summary>
    public class HttpRequest
    {
        private byte[] _body = new byte[] { };
        private TransformHeader _headers;
        private FileCollection _files;
        private RequestCookie _cookies;

        /// <summary>
        /// Э������,�ο�<see cref="ProtocolType"/>���г���ֵ
        /// </summary>
        public ProtocolType Protocol { get; protected set; }
        /// <summary>
        /// Э��汾
        /// </summary>
        public HttpVersion ProtocolVersion { get; protected set; }
        /// <summary>
        /// ����·��
        /// </summary>
        public string Path { get; protected set; }
        /// <summary>
        /// ��ѯ�ַ���
        /// </summary>
        public string QueryString { get; protected set; }
        /// <summary>
        /// ��ѯ �����ɺ��Դ�Сд
        /// </summary>
        public Dictionary<string, string> Query = new Dictionary<string, string>(new StringCompareIgnoreCase());
        /// <summary>
        /// POST �����ɺ��Դ�Сд
        /// </summary>
        public Dictionary<string, string> Post = new Dictionary<string, string>(new StringCompareIgnoreCase());
        /// <summary>
        /// �ɽ���ѹ���㷨
        /// </summary>
        public string AcceptEncoding { get; protected set; }
        /// <summary>
        /// �����ַ
        /// </summary>
        public string Host { get; protected set; }
        /// <summary>
        /// �ͻ�����Ϣ
        /// </summary>
        public string UserAgent { get; protected set; }
        /// <summary>
        /// ���󷽷�
        /// </summary>
        public RequestMethod Method { get; protected set; }
        /// <summary>
        /// ��������
        /// </summary>
        public string ContentType { get; protected set; }
        /// <summary>
        /// ���ݱ�������
        /// </summary>
        public string ContentCharset { get; protected set; }
        /// <summary>
        /// ���ݴ�С
        /// </summary>
        public string ContentLength { get; protected set; }
        /// <summary>
        /// ����ͷ
        /// </summary>
        public TransformHeader Headers { get { return _headers; } protected set { _headers = value; } }
        ///// <summary>
        ///// ������ͨѶ����
        ///// </summary>
        //public byte[] PackData { get; protected set; }
        /// <summary>
        /// ԭʼ������������
        /// </summary>
        public byte[] RequestLineData { get; protected set; }
        /// <summary>
        /// ԭʼ���������ַ���
        /// </summary>
        public string RequestLineString { get; protected set; }
        /// <summary>
        /// ����ͷ����
        /// </summary>
        public byte[] HeaderData { get; protected set; }
        /// <summary>
        /// ���������壬��Ч��������
        /// </summary>
        public byte[] Body { get { return _body; } protected set { _body = value; } }
        /// <summary>
        /// �ļ�
        /// </summary>
        public FileCollection Files { get { return _files; } protected set { _files = value; } }
        /// <summary>
        /// Cookie
        /// </summary>
        public RequestCookie Cookies { get { return _cookies; } protected set { _cookies = value; } }
        /// <summary>
        /// �ͻ���IP��ַ
        /// </summary>
        public string ClientAddress { get; internal set; }
        /// <summary>
        /// �ͻ����Ƿ���ͨ����֤
        /// </summary>
        public bool IsAuthorized { get; internal set; }
        /// <summary>
        /// �ͻ������ܵ�����ѡ��
        /// </summary>
        public LanguagePriority[] AcceptLanguage { get; private set; }
        /// <summary>
        /// �������Դ��ַ
        /// </summary>
        public string Referer { get; private set; }
        /// <summary>
        /// ���캯��
        /// </summary>
        public HttpRequest()
        {
            //Ĭ��HTTP/1.1
            ProtocolVersion = HttpVersion.Version11;
            _headers = new TransformHeader();
            _files = new FileCollection();
            _cookies = new RequestCookie();
            _body = new byte[] { };
        }
        /// <summary>
        /// �����������ݰ�
        /// <code>
        ///                     
        ///  GET / HTTP/1.1\r\nHost: 127.0.0.1:9000\r\n
        ///  User-Agent: Mozilla/5.0 (Windows NT 5.2; rv:22.0) Gecko/20100101 Firefox/22.0\r\n
        ///  Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n
        ///  Accept-Language: zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3\r\n
        ///  Accept-Encoding: gzip, deflate\r\n
        ///  Authorization: Basic c2Rmc2RmOnNmc2Rm\r\n
        ///  Connection: keep-alive\r\n
        ///  Cache-Control: max-age=0
        ///  
        /// </code>
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public static HttpRequest Parse(byte[] data)
        {
            HttpRequest req = new HttpRequest();
            //����ͷ
            int posCR = 0;
            int posCaret = 0;
            int count = 0;

            int indLine = 0;
            int dataLength = data.Length;

            while ((posCR < dataLength) && Array.IndexOf(data, ASCIICode.CR, posCR + 1) > 0)
            {
                posCR = Array.IndexOf(data, ASCIICode.CR, posCR + 1);

                //��������CR
                byte[] fragement = new byte[posCR - posCaret];
                Array.Copy(data, posCaret, fragement, 0, posCR - posCaret);
                if (indLine == 0)
                {
                    ParseRequestLine(ref req, fragement);
                }
                else
                {
                    ParseHeaders(ref req, fragement);
                }

                if ((Array.IndexOf(data, ASCIICode.CR, posCR + 1) == posCR + 2))
                {
                    break;
                }
                //�����ָ��ֽڶ�
                posCaret = posCR + 2;
                indLine++;
                //TODO �ÿն���
            }

            //ͷ��Ϣ����
            //HOST
            ParseHeaderHost(ref req);
            //User-Agent
            ParseHeaderUserAgent(ref req);
            //Accept-Language
            ParseHeaderAcceptLanguage(ref req);
            //Referer
            ParseHeaderReferer(ref req);
            //Content-Type
            ParseHeaderContentType(ref req);
            //Range
            ParseHeaderRange(ref req);
            //����Cookie
            ParseCookie(ref req);
            //TODO �˴��Ƿ���Ҫ�ֱ�GET/POST
            //�������� ���ز���
            if (data.Length > posCR + 4)
            {
                req.Body = new byte[data.Length - (posCR + 4)];
                //TODO �˴�����������һ�����ݶ��󣬵����ڴ�ռ�ù���
                Array.Copy(data, posCR + 4, req.Body, 0, req.Body.Length);
                ParsePayload(ref req, req.Body);
            }
            return req;
        }
        //TODO HTTP/2.0 �ǻ��ڶ���������֡���˴���Ҫ��������
        //TODO ���жϰ����Ƿ񾭹�ѹ��
        /// <summary>
        /// ��������������
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        private static void ParsePayload(ref HttpRequest req, byte[] data)
        {
            string formType = req.Headers.GetValue(HeaderProperty.ContentType.PropertyName);
            if (formType != null)
            {
                if (formType.Contains("application/x-www-form-urlencoded"))
                {
                    ParsePayloadFormUrl(ref req,ref data);
                }
                else if (formType.Contains("multipart/form-data"))
                {
                    ParsePayloadFormData(ref req,ref data);
                }
                else
                {
                    ParsePayloadText(ref req,ref data);
                }
            }
        }
        /// <summary>
        /// ���������� application/x-www-form-urlencoded
        /// ���������������ڲ�ѯ�ַ���
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        private static void ParsePayloadFormUrl(ref HttpRequest req,ref byte[] data)
        {
            req.Post = UrlEncoder.ParseQuery(StringEncoder.Decode(data));
        }
        //TODO �ļ���Ӧд�뻺����,���������ڴ�ռ�ù���
        //DONE �˴����ܽ���һ���ļ��������޸Ĵ���
        //TODO multipart/form-data Ҳ���ύ�ı����ݣ��˴�δ�ܺܺõĴ���
        /// <summary>
        /// ���������� multipart/form-data
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        /// <example>
        /// <code>
        ///   --{boundary}
        ///   Content-Disposition: form-data; name="{file.fieldname}"; filename="{file.name}"\r\n
        ///   Content-Type: application/octet-stream\r\n
        ///   \r\n
        ///   {file.binary}\r\n
        ///   \r\n
        ///   --{boundary}\r\n
        ///   Content-Disposition: form-data; name="{file.fieldname}"; filename="{file.name}"\r\n
        ///   Content-Type: application/octet-stream\r\n
        ///   \r\n
        ///   {file.binary}\r\n
        ///   --{boundary}\r\n
        ///   Content-Disposition: form-data; name="{field.name}"\r\n
        ///   \r\n
        ///   --{boundary}--\r\n
        /// </code>
        /// </example>
        private static void ParsePayloadFormData(ref HttpRequest req,ref byte[] data)
        {
            string contentType = req.Headers.GetValue(HeaderProperty.ContentType.PropertyName);
            string boundary = "";
            //�˴����ã��ָ���ͨ����
            string[] values = contentType.Split(new[] { ((char)ASCIICode.SEMICOLON).ToString() }, StringSplitOptions.RemoveEmptyEntries);

            //ȡ�÷ָ����boundary
            foreach (var s in values)
            {
                if (s.Trim().StartsWith("boundary"))
                {
                    boundary = s.Trim().Replace("boundary=", "");
                    break;
                }
            }

            byte[] bBound = StringEncoder.Encode(boundary);

            //�ָ� form-data
            int indBoundary = -1, indFragFirst/*�ָ����*/ = 0, indFragNext/*�¶ηָ����*/ = 0;

            while ((indBoundary + 1) < data.Length && Array.IndexOf(data, ASCIICode.MINUS, indBoundary + 1) >= 0)
            {
                try
                {
                    indFragFirst = indFragNext;
                    indBoundary = Array.IndexOf(data, ASCIICode.MINUS, indBoundary + 1);

                    //ѭ�����ָ��
                    bool isFragStart = true;
                    bool isFragEnd = false;

                    for (int i = 0; i < bBound.Length; i++)
                    {
                        if ((data.Length >= indBoundary + bBound.Length) && data.Length >= (indBoundary + i + 2 - 1) && data[indBoundary + i + 2] != bBound[i])
                        {
                            isFragStart = false;
                        }
                    }
                    if (isFragStart)
                    {
                        indFragNext = indBoundary;
                        //�����ָ�
                        indBoundary += bBound.Length + 2 + 2;
                    }

                    //�ָ���Ϣ��
                    if (isFragStart && indFragNext > 0)
                    {
                        //Console.WriteLine("����Ƭ��{0}-{1},����{2} Byte", indFragFirst, indFragNext, indFragNext - indFragFirst);
                        int posCR = 0, posCaret/*�ָ���ʼλ*/ = 0, index = 0;

                        byte[] fragment = new byte[indFragNext - indFragFirst];

                        Array.Copy(data, indFragFirst, fragment, 0, indFragNext - indFragFirst);

                        bool isFile = false;

                        string fieldName = string.Empty;
                        string fileName = string.Empty;

                        ///<example>
                        ///-----------------------------97671069125495\r\n
                        ///Content-Disposition: form-data; name=\"mailaddress\"\r\n
                        ///\r\n
                        ///abcdefg
                        ///\r\n
                        ///</example>

                        //��ȡ�ֶ�ͷ����
                        while ((posCR = Array.IndexOf(fragment, ASCIICode.CR, posCR + 1)) > 0)
                        {
                            //TODO ����ͨ�ֶεĴ���������

                            byte[] fraghead = new byte[posCR - posCaret];
                            Array.Copy(fragment, posCaret, fraghead, 0, posCR - posCaret);

                            //Content-Disposition
                            if (index == 1)
                            {
                                //TODO �ļ���������������⣬��ν�����ļ����ı����ʽ��ͬ
                                //����������Ϣ
                                //Content-Disposition: form-data; name="{field.name}"; filename="{file.name}"
                                string disposition = StringEncoder.Decode(fraghead);

                                string[] headers = disposition.Split(new[] { ((char)ASCIICode.SEMICOLON).ToString() }, StringSplitOptions.RemoveEmptyEntries);
                                fieldName = headers[1].Trim().Replace("name=", "").Trim((char)ASCIICode.QUOTE);

                                if (headers.Length > 2)
                                {
                                    fileName = headers[2];
                                    if (fileName.Contains("filename="))
                                    {
                                        isFile = true;
                                        fileName = fileName.Trim().Replace("filename=", "").Trim((char)ASCIICode.QUOTE);
                                    }
                                }
                            }
                            //Content-Type
                            if (index == 2)
                            {

                            }

                            //�����ָ��ֽڶ�
                            posCaret = posCR + 2;
                            index++;

                            //��������CR
                            if (Array.IndexOf(fragment, ASCIICode.CR, posCR + 1) != posCR + 2)
                            {

                            }
                            else
                            {
                                break;
                            }
                        }
                        //��������
                        if (data.Length > posCR + 4)
                        {
                            //����ĩ�˻��������ַ�(\r\n)���ʴ˴��ټ������ֽ�
                            var postField = new byte[fragment.Length - (posCR + 4 + 2)];
                            Array.Copy(fragment, posCR + 4, postField, 0, postField.Length);

                            if (isFile)
                            {
                                File file = new File
                                {
                                    FileName = HtmlEncoder.EntityCodeToString(fileName),
                                    FileData = postField,
                                    FileIndex = req.Files.Count,
                                    FieldName = fieldName
                                };
                                //file.FileTempSavePath = AppDomain.CurrentDomain.BaseDirectory + @"Temp\" + file.FileName.ToString();
                                req.Files.Add(file);
                                ////д����ʱĿ¼
                                //FileStream fs = new FileStream(file.FileTempSavePath, FileMode.OpenOrCreate);
                                //int length = fragbody.Length - (posCR + 4);
                                //int blockSize = 1024;
                                //int count = length / blockSize;
                                //byte[] blockData = new byte[blockSize];
                                //int mode = length % blockSize;
                                //for (int i = 0; i < count; i++)
                                //{
                                //    Array.Copy(fragbody, posCR + 4 + i * blockData.Length, blockData, 0, blockData.Length);
                                //    fs.Write(blockData, 0, blockData.Length);
                                //}
                                //if (mode > 0)
                                //{
                                //    Array.Resize(ref blockData, mode);
                                //    Array.Copy(fragbody, posCR + 4 + blockSize * count, blockData, 0, blockData.Length);
                                //    fs.Write(blockData, 0, blockData.Length);
                                //}
                                //fs.Flush();
                                //fs.Dispose();
                            }
                            else
                            {
                                //var postField = new byte[fragbody.Length - (posCR + 4)];
                                req.Query.Add(fieldName, StringEncoder.Decode(postField));
                            }
                            //TODO �ÿն���
                            fragment = null;
                            postField = null;
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Exception occurs while parsing multipart/form-data:{0}", ex.Message);
                }
            }
        }
        //TODO ������չʵ�֣���ʱ��ʵ��
        /// <summary>
        /// ���������� �ı�����
        ///     application/json
        ///     text/plain
        ///     text/xml
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        private static void ParsePayloadText(ref HttpRequest req,ref byte[] data)
        {

        }
        /// <summary>
        /// ����ͷ����
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        private static void ParseHeaders(ref HttpRequest req, byte[] data)
        {
            HeaderProperty hp = HeaderProperty.Parse(data);
            req.Headers.Add(hp.PropertyName, hp.PropertyValue);
            #if DEBUG
                Console.WriteLine("{0}:{1}",hp.PropertyTag,hp.PropertyValue);
            #endif
        }
        /// <summary>
        /// ����UserAgent
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderUserAgent(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.UserAgent.PropertyName))
            {
                req.UserAgent = req.Headers.GetValue(HeaderProperty.UserAgent.PropertyName);
            }
        }
        /// <summary>
        /// ��������Ŀ��������ַ
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderHost(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.Host.PropertyName))
            {
                req.Host = req.Headers.GetValue(HeaderProperty.Host.PropertyName);
            }
        }
        /// <summary>
        /// ������Դҳ���ַ
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderReferer(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.Referer.PropertyName))
            {
                req.Referer = req.Headers.GetValue(HeaderProperty.Referer.PropertyName);
            }
        }
        /// <summary>
        /// ����������������
        /// 	�﷨��<code>zh-cn,zh;q=0.8,en-us;q=0.5,en;q=0.3</code>
        /// 	�ָ����Ϊ,
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderAcceptLanguage(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.AcceptLanguage.PropertyName))
            {
                var language = req.Headers.GetValue(HeaderProperty.AcceptLanguage.PropertyName) ?? "";
                var languages = language.Split(new char[] { (char)ASCIICode.COMMA }, StringSplitOptions.RemoveEmptyEntries);
                req.AcceptLanguage = new LanguagePriority[languages.Length];
                try
                {
                    for (int i = 0; i < languages.Length; i++)
                    {
                        var lan = languages[i];
                        var lans = lan.Split(new char[] { (char)ASCIICode.COMMA }, StringSplitOptions.RemoveEmptyEntries);
                        req.AcceptLanguage[i] = new LanguagePriority()
                        {
                            LanguageName = lans[0],
                            Weight = lans.Length > 1 ? int.Parse(lans[1]) : 1
                        };
                    }
                }
                catch
                {

                }
            }
        }
        /// <summary>
        /// �����ĵ�����
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderContentType(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.ContentType.PropertyName))
            {
                var contentType = req.Headers.GetValue(HeaderProperty.ContentType.PropertyName);
                string[] cts = contentType.Split(new[] { ((char)ASCIICode.SEMICOLON).ToString() + ((char)ASCIICode.SPACE).ToString() }, StringSplitOptions.RemoveEmptyEntries);
                if (cts.Length > 0)
                {
                    req.ContentType = cts[0];
                    if (cts.Length > 2)
                    {
                        req.ContentCharset = cts[2];
                    }
                }
            }
        }
        /// <summary>
        /// �����ĵ�����Χ 
        /// <para>
        ///     �˹�����ҪӦ���ڶϵ�����
        /// </para>
        /// </summary>
        /// <param name="req"></param>
        private static void ParseHeaderRange(ref HttpRequest req)
        {

        }
        /// <summary>
        /// ����Cookie
        /// </summary>
        /// <param name="req"></param>
        private static void ParseCookie(ref HttpRequest req)
        {
            if (req.Headers.Contains(HeaderProperty.Cookie.PropertyName))
            {
                req.Cookies = RequestCookie.Parse(req.Headers.GetValue(HeaderProperty.Cookie.PropertyName));
            }
        }

        /// <summary>
        /// ������������
        /// ���� ��ѯ Э��
        /// </summary>
        /// <param name="req"></param>
        /// <param name="data"></param>
        private static void ParseRequestLine(ref HttpRequest req, byte[] data)
        {
            //������ʼ��
            req.RequestLineData = data;
            req.RequestLineString = Encoding.UTF8.GetString(data);
            string[] sFirst = req.RequestLineString.Split(new[] { (char)ASCIICode.SPACE }, StringSplitOptions.None);
            //���� ��ѯ Э�� 
            string sMethod = sFirst[0];
            string sUrl = sFirst[1];
            string sProtocol = sFirst[2];

            RequestMethod rm = AbsClassEnum.Get<RequestMethod>(sMethod);
            req.Method = rm;

            //�жϷ����Ƿ�����֪����
            if (Equals(req.Method,null))
            {
                req.Method = new RequestMethod(sMethod);
            }
            string[] urls = sUrl.Split(new[] { (char)ASCIICode.QUESTION }, StringSplitOptions.RemoveEmptyEntries);
            req.Path = urls[0];
            if (urls.Length > 1)
            {
                req.QueryString = urls[1];
                req.Query = UrlEncoder.ParseQuery(urls[1]);
            }

            string sProtoType = sProtocol.Substring(0, sProtocol.IndexOf((char)ASCIICode.DIVIDE));
            string sProtoVersion = sProtocol.Substring(sProtocol.IndexOf((char)ASCIICode.DIVIDE) + 1);

            req.Protocol = AbsClassEnum.Get<ProtocolType>(sProtoType);
            req.ProtocolVersion = AbsClassEnum.Get<HttpVersion>(sProtoVersion);

        }
        //TODO �˹�����Ҫ�����Խ�����֤
        /// <summary>
        /// �����ز�
        /// </summary>
        /// <param name="headerKeyNameUpperCase">ͷ����������д</param>
        /// <returns></returns>
        public byte[] GetBuffer(bool headerKeyNameUpperCase)
        {
            List<byte> data = new List<byte>();
            //ע��״̬��Ϣ
            data.AddRange(GetRequestLine());
            data.AddRange(TransformHeader.Carriage);
            //ע��Ĭ��ͷ��
            data.AddRange(_headers.GetBuffer(headerKeyNameUpperCase));
            //ע��Cookie
            data.AddRange(_cookies.GetBuffer());
            //ע��ָ��
            data.AddRange(TransformHeader.Carriage);
            //ע����Ӧ����
            data.AddRange(_body);
            return data.ToArray();
        }
        /// <summary>
        /// �����ز�
        /// </summary>
        /// <returns></returns>
        public byte[] GetBuffer()
        {
            return GetBuffer(false);
        }
        /// <summary>
        /// ����ͷ��Ϣ
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public HttpRequest SetHeader(string key,string value)
        {
            _headers.Add(key, value);
            return this;
        }
        /// <summary>
        /// ����ͷ��Ϣ
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public HttpRequest SetHeader(HeaderProperty key,string value)
        {
            _headers.Add(key, value);
            return this;
        }
        /// <summary>
        /// Ӧ��ͷ��Ϣ���� ��������ͷ����
        /// </summary>
        /// <param name="headers"></param>
        /// <returns></returns>
        public HttpRequest SetHeaders(TransformHeader headers)
        {
            if (headers != null)
            {
                _headers = headers;
            }
            return this;
        }
        /// <summary>
        /// ��������·��
        /// </summary>
        /// <param name="path"></param>
        /// <returns></returns>
        public HttpRequest SetPath(string path)
        {
            Path = path;
            return this;
        }
        /// <summary>
        /// �������󷽷�
        /// </summary>
        /// <param name="method"></param>
        /// <returns></returns>
        public HttpRequest SetMethod(RequestMethod method)
        {
            Method = method;
            return this;
        }
        /// <summary>
        /// ��������HTTPЭ��
        /// </summary>
        /// <param name="version"></param>
        /// <returns></returns>
        public HttpRequest SetProtocol(HttpVersion version)
        {
            ProtocolVersion = version;
            return this;
        }
        /// <summary>
        /// ����Body����
        /// </summary>
        /// <param name="data"></param>
        /// <returns></returns>
        public HttpRequest SetBody(byte[] data)
        {
            if (data != null)
            {
                _body = data;
                ContentLength = _body.Length.ToString();
                SetHeader(HeaderProperty.ContentLength, data.Length.ToString());
            }
            return this;
        }
        /// <summary>
        /// ����������
        /// </summary>
        /// <returns></returns>
        public byte[] GetRequestLine()
        {
            return StringEncoder.Encode(string.Format("{0} {1} HTTP/{2}", Method.Name, Path, ProtocolVersion.Version));
        }
        /// <summary>
        /// ����URI��Ϣ���ֱ�ע�뵽<see cref="HeaderProperty.Host"/>��<see cref="HeaderProperty.Referer"/>����ͷ������
        /// </summary>
        /// <param name="uri"></param>
        /// <returns></returns>
        public HttpRequest SetUri(UriInfo uri)
        {
            //ע��Host
            SetHeader(HeaderProperty.Host, string.IsNullOrEmpty(uri.Domain) ? uri.Host : uri.Domain);
            SetHeader(HeaderProperty.Referer, uri.Url);
            return this;
        }
        ~HttpRequest()
        {
            //PackData = null;
            RequestLineData = null;
            _body = null;
            _headers = null;
            _cookies = null;
            HeaderData = null;
            _files = null;
            AcceptLanguage = null;
        }
    }
    /// <summary>
    /// HttpRequest��չ
    /// </summary>
    public static class HttpRequestExtension
    {
        
    }
}
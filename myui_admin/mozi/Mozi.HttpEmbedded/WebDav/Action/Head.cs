using Mozi.HttpEmbedded.Source;
using Mozi.HttpEmbedded.WebDav.Storage;

namespace Mozi.HttpEmbedded.WebDav.Method
{
    /// <summary>
    ///  <c>PROPFIND</c> WebDAV��չ����
    /// </summary>
    internal class WebDavHeadMethodHandler : MethodHandlerBase, IMethodHandler
    {
        /// <summary>
        /// ��Ӧ����
        /// </summary>
        /// <param name="server"><see cref="WebDAVServer" /> </param>
        /// <param name="context"> 
        /// <see cref="HttpContext" /> 
        ///  </param>
        /// <param name="store"><see cref="IWebDavStore" /> <see cref="WebDAVServer" /></param>
        /// <exception cref="WebDavNotFoundException"><para>
        ///   <paramref name="context" /> </para>
        /// <para>- or -</para>
        /// <para>
        ///   <paramref name="context" /> </para></exception>
        /// <exception cref="WebDavConflictException"><paramref name="context" /> </exception>
        public StatusCode Invoke(WebDAVServer server, HttpContext context, IWebDavStore store)
        {
            //����Ŀ¼��Դ�嵥
            IWebDavStoreCollection collection = GetParentCollection(store, context.Request.Path);

            //ȡ��Դ�嵥
            IWebDavStoreItem item = GetItemFromCollection(collection, context.Request.Path);

            context.Response.AddHeader(HeaderProperty.ContentType.PropertyName, Mime.Default);
            context.Response.AddHeader(HeaderProperty.LastModified.PropertyName, item.ModifyDate.ToUniversalTime().ToString("R"));

            return StatusCode.Success;
        }
    }
}
using Mozi.HttpEmbedded.Source;

namespace Mozi.HttpEmbedded.WebDav.Storage.Base
{
    public class WebDavStoreDocumentBase : WebDavStoreItemBase
    {
        protected WebDavStoreDocumentBase(IWebDavStoreCollection parentCollection, string name) : base(parentCollection, name)
        {
        }

        public string MimeType
        {
            get
            {
                //TODO �˴���Ҫ����
                return Mime.GetContentType(Name);
            }
        }
    }
}
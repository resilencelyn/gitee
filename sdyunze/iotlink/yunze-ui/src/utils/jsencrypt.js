import JSEncrypt from 'jsencrypt/bin/jsencrypt.min'

// 密钥对生成 http://web.chacuo.net/netrsakeypair

/*const publicKey = 'MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBAKoR8mX0rGKLqzcWmOzbfj64K8ZIgOdH\n' +
  'nzkXSOVOZbFu/TJhZ7rFAN+eaGkl3C4buccQd/EjEsj9ir7ijT7h96MCAwEAAQ=='

const privateKey = 'MIIBVAIBADANBgkqhkiG9w0BAQEFAASCAT4wggE6AgEAAkEAqhHyrootSsYourNxaY\n' +
  '7Nt+PrgrxkiA50efORdI5U5lsW79MmFnusUA355oaSXc123123u5xxB38SMSyP2KvuKN\n' +
  'PuH3owIDAQABAkAfoiLyL+Z4lf4Myxk6xUDgLaWGximj20CUf+5BKKnlrK+Ed8gA\n' +
  'kM0HqoTt2UZwA5E2MzS4EI2gjfQhz5X28uqxAiEA3wNFxfrCZlSZHb0gn2zDpWow\n' +
  'cSxQAgiCstxGUoOqlW8CIQDDOerGKH5OmCJ4Z21v+F25WaHYPxCFMvwxpcw99Ecv\n' +
  'DQIgIdhDTIqD2jfYjPTY8Jj3EDGPbH2HHuffvflECt3Ek60CIQCFRlCkHpi7hthh\n' +
  'YhovyloRYsM+IS9h/0BzlEAuO0ktMQIgSPT3aFAgJYwKpqRYKlLDVcflrootCKY7u3\n' +
  'UP8iWi1Qw0Y='*/

const publicKey ='MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCcHDbDfxkaicAgW+K5vo0fcjrK\n'+
  'P3G/6dtDpAeycLof4ZdtThnGUiXqvTbZ63vJ+kccm/mnoPaOe9QvyqHWC+z3jooM\n'+
  'hSlPSDSqSkCOJyD+Kd/4+YbpJd310EqSHso7CTF2rIVxD1dYTOfqB7pfqgGT8+a+\n'+
  'friCBSwbcSbr9gD20wIDAQAB';


const privateKey = 'MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBAJwcNsN/GRqJwCBb\n'+
  '4rm+jR9yOso/cb/p20OkB7Jwuh/hl21OGcZSJeq9Ntnre8n6Rxyb+aeg9o571C/K\n'+
  'odYL7PeOigyFKU9INKpKQI4nIP4p3/j5hukl3fXQSpIeyjsJMXashXEPV1hM5+oH\n'+
  'ul+qAZPz5r5+uIIFLBtxJuv2APbTAgMBAAECgYEAhkC5VA004J7AjNOYu1bFdNq+\n'+
  '9nnzcDFEsPoa6HmLh6lxmb4F6olxNFv/FRAwCDZl+5Oj82Y5Q7X4eBVScosVAUUz\n'+
  '2CGQf4V9kGZuY2Mc7F624NCDqkfZIAyoF28lEbYfFILKfIfT4iUrRI1c7HWipwQB\n'+
  'xDsiKoaY/1s2QIL75wECQQDMgFd78MEW/oJtp2lETL8ynbkzMICHzBAqgjtZyz7m\n'+
  '7BH4g+u9vf2qTHbXWYZj0upUnVLgfPEreMyWOMqMyWxTAkEAw2w5j3c9wWEBv6JH\n'+
  'd7p2aLF5juTWdTh0sJ1tyb2D2QxM3Kv7STZc0wLSSPn3IIzRf9Cz4AKgg0ZL86n6\n'+
  'EgT7gQJBAK9RxC6cW12UOks5v/Ru+2ewckrhZP+GwcE/vxQfO8ELV+i/aEZV8Hek\n'+
  'cCn1NVK1SnHaImgUlUmYmYScEYIbWMMCQDrXWlmFSNXyNImT/jrhrQeG8JgC9UPk\n'+
  'VNZdAJKu+oUe/AGx0v7PXJG4lsKcaTmmzOnUWGTJxbThNLtQYdiNIYECQFOseGzj\n'+
  'jGc01DRsmBZvxXpHzYT/9jDCD69u1iT5DAf8Yyd7LTOZ7bBrGUgtTcWjghn8XXLR\n'+
  '6OhLTmvpgt9DIHY=';



// 加密
export function encrypt(txt) {
  const encryptor = new JSEncrypt()
  encryptor.setPublicKey(publicKey) // 设置公钥
  return encryptor.encrypt(txt) // 对数据进行加密
}

// 解密
export function decrypt(txt) {
  const encryptor = new JSEncrypt()
  encryptor.setPrivateKey(privateKey) // 设置私钥
  return encryptor.decrypt(txt) // 对数据进行解密
}


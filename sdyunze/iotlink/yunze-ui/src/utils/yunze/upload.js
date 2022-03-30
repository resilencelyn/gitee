// An highlighted block
import COS from 'cos-js-sdk-v5'
/*import { getSecret } from '@/api/upload'*/

export default function upload(file, config, callback) {
  if (typeof config.isPublic === 'undefined') {
    config.isPublic = true
  }
  console.log(config);
/* getSecret({
    type: config.type,
    path: config.path,
    isPublic: config.isPublic ? config.isPublic : false
  }).then(response => {
    const data = response.data
    const cos = new COS({
      getAuthorization: function(options, callback) {
        callback({
          TmpSecretId: data.tmpSecretId,
          TmpSecretKey: data.tmpSecretKey,
          XCosSecurityToken: data.sessionToken,
          ExpiredTime: data.expiredTime
        })
      }
    })

    return {
      cos: cos,
      dir: data.url,
      bucket: data.bucket,
      region: data.region
    }
  }).then(data => {
    const cos = data.cos
    const dir = data.dir
    const bucket = data.bucket
    const region = data.region

    const url = cos.getObjectUrl({
      Bucket: bucket,
      Region: region,
      Key: `${dir}/${file.name}`,
      Sign: false
    }, function(data) {
      cos.putObject({
        Bucket: bucket,
        Region: region,
        Key: `${dir}/${file.name}`,
        Body: file
      }, function(data) {
        if (config.isPublic) {
          callback(url.replace(/^http(s)?:\/\/(.*?)\//, 'https://www.baidu.com/'))
        } else {
          callback(url)
        }
      })
    })
  })*/
}


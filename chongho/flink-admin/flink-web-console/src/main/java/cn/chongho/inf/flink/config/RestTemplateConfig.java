package cn.chongho.inf.flink.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.ClientHttpResponse;
import org.springframework.http.client.HttpComponentsClientHttpRequestFactory;
import org.springframework.web.client.DefaultResponseErrorHandler;
import org.springframework.web.client.RestClientException;
import org.springframework.web.client.RestTemplate;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

/**
 * @author ming
 */
@Configuration
public class RestTemplateConfig {

    @Bean
    public RestTemplate restTemplate(){
        RestTemplate restTemplate = new RestTemplate();
        restTemplate.setErrorHandler(new DefaultResponseErrorHandler(){
            @Override
            public void handleError(ClientHttpResponse response) throws IOException {
//                if(response.getStatusCode() != HttpStatus.INTERNAL_SERVER_ERROR) {
//                    super.handleError(response);
//                }else{
                StringBuilder sb = new StringBuilder();
                String line;
                BufferedReader br = new BufferedReader(new InputStreamReader(response.getBody()));
                while ((line = br.readLine()) != null) {
                    sb.append(line);
                }
                throw new RestClientException(sb.toString());
//                }
            }
        });
        return restTemplate;
    }

    @Bean
    public RestTemplate restTemplateByRequestFactory(){
        RestTemplate restTemplate = new RestTemplate(new HttpComponentsClientHttpRequestFactory());
        restTemplate.setErrorHandler(new DefaultResponseErrorHandler(){
            @Override
            public void handleError(ClientHttpResponse response) throws IOException {
//                if(response.getStatusCode() != HttpStatus.INTERNAL_SERVER_ERROR) {
//                    super.handleError(response);
//                }else{
                StringBuilder sb = new StringBuilder();
                String line;
                BufferedReader br = new BufferedReader(new InputStreamReader(response.getBody()));
                while ((line = br.readLine()) != null) {
                    sb.append(line);
                }
                throw new RestClientException(sb.toString());
//                }
            }
        });
        return restTemplate;
    }
}

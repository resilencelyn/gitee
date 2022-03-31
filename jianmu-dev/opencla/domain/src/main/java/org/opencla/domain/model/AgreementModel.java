package org.opencla.domain.model;

import lombok.Getter;
import lombok.NoArgsConstructor;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;
import java.time.LocalDateTime;

@Getter
@NoArgsConstructor
@Entity
@Table(name = "agreement")
public class AgreementModel {
    /**
     * 邮箱
     */
    @Id
    @Column(length = 320, nullable = false)
    private String email;
    /**
     * 签署时间
     */
    @Column(nullable = false)
    private final LocalDateTime signedTime = LocalDateTime.now();
    /**
     * 协议版本
     */
    @Column(length = 20, nullable = false)
    private String agreementVersion;

    public AgreementModel(String email, String agreementVersion) {
        this.email = email;
        this.agreementVersion = agreementVersion;
    }
}

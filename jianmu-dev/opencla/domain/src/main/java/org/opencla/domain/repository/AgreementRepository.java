package org.opencla.domain.repository;

import org.opencla.domain.model.AgreementModel;
import org.springframework.data.jpa.repository.JpaRepository;

public interface AgreementRepository extends JpaRepository<AgreementModel, String> {
}

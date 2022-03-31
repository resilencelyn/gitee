package org.opencla.api.config.valid_group;

import javax.validation.GroupSequence;

@GroupSequence({NotBlankGroup.class, PatternGroup.class})
public interface SequenceGroup {
}

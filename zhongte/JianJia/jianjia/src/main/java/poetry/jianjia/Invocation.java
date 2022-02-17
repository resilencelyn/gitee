package poetry.jianjia;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class Invocation {
  public static Invocation of(Method method, List<?> arguments) {
    Utils.checkNotNull(method, "method == null");
    Utils.checkNotNull(arguments, "arguments == null");
    return new Invocation(method, new ArrayList<>(arguments)); // Defensive copy.
  }

  private final Method method;
  private final List<?> arguments;

  /** Trusted constructor assumes ownership of {@code arguments}. */
  Invocation(Method method, List<?> arguments) {
    this.method = method;
    this.arguments = Collections.unmodifiableList(arguments);
  }

  public Method method() {
    return method;
  }

  public List<?> arguments() {
    return arguments;
  }

  @Override public String toString() {
    return String.format("%s.%s() %s",
        method.getDeclaringClass().getName(), method.getName(), arguments);
  }
}

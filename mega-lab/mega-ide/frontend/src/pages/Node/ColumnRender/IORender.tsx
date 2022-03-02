import { autoFormatHardwareSize } from "@/utils/utils";
import ColumnRender from ".";
import Metric from "./Metric";

export default (props: {
  nodeName: string;
  metrics: string[];
}) => {
  const { nodeName, metrics } = props;

  if (metrics.length !== 1) return <div>---</div>;

  return (
    <Metric
      nodeName={nodeName}
      metrics={metrics}
      render={(data) => {
        const value = data[metrics[0]];
        let main;
        if (!value) main = '- B/s';
        const size = autoFormatHardwareSize({ value: Number(value) });
        return (
          <ColumnRender
            main={`${size.value.toFixed(2)} ${size.unit}/s`}
          />
        )
      }}
    />
  )
}
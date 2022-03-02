import ColumnRender from ".";
import Metric from "./Metric";

export default (props: {
  nodeName: string;
  metrics: string[];
  format?: (value: number) => number;
  suffix?: string;
}) => {
  const { nodeName, metrics, format, suffix = '' } = props;

  if (metrics.length !== 3) return <div>---</div>;

  return (
    <Metric
      nodeName={nodeName}
      metrics={metrics}
      render={(data) => {
        const percent = data[metrics[0]] ? Number(data[metrics[0]]) * 100 : undefined;
        const used = data[metrics[1]] ? (format ? format(Number(data[metrics[1]])) : Number(data[metrics[1]])) : undefined;
        const total = data[metrics[2]] ? (format ? format(Number(data[metrics[2]])) : Number(data[metrics[2]])) : undefined;
        return (
          <ColumnRender
            main={`${typeof percent !== 'undefined' ? percent.toFixed(0) : '-'}%`}
            sub={`${typeof used !== 'undefined' ? used.toFixed(2) : '-'}/${typeof total !== 'undefined' ? total.toFixed(2) : '-'} ${suffix}`}
          />
        )
      }}
    />
  )
}
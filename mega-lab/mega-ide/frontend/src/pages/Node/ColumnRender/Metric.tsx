import { useEffect, useState } from "react";
import ColumnRender from ".";
import { getMetric } from "../api";

interface IData {
  [key: string]: string[]
}

export default (props: {
  nodeName: string;
  metrics: string[];
  render: (data: IData) => JSX.Element;
}) => {
  const { nodeName, metrics, render } = props;

  const [data, setData] = useState<IData>();

  const getData = async () => {
    const res = await getMetric([nodeName], metrics);
    if (!res || !res.data) {
      return;
    }
    setData(res?.data);
  }

  useEffect(() => {
    getData();
    const interval = setInterval(getData, 5000);
    return () => {
      if (interval) clearInterval(interval);
    }
  }, [])

  if (!data) return <div>---</div>;

  return render(data);
}
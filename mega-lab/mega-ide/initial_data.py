import os

from logzero import logger
import pandas as pd

from server.framework.core.database import SessionLocal, engine


def main() -> None:
    logger.info("Creating initial data")

    session = SessionLocal()
    init_data_path = os.path.join(os.path.dirname(__file__), "init_data")
    files = ['IdeRegistry.csv', 'ide_image.csv']
    for file in files:
        file_path = os.path.join(init_data_path, file)
        df = pd.read_csv(file_path, sep=",")
        if file == "menu.csv":
            df['component'] = df['component'].apply(lambda x: '' if pd.isnull(x) else x)
            df['name'] = df['name'].apply(lambda x: '' if pd.isnull(x) else x)
        logger.info(f"{file}  load successed")
        table_name = file.replace(".csv", "")
        df.to_sql(table_name, engine, if_exists="append", index=False)
    session.commit()
    session.close()

    logger.info("Initial data created")


if __name__ == "__main__":
    main()

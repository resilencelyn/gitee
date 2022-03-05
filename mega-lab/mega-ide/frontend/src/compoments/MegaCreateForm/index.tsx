import React, { FC, useEffect } from 'react';
import { ModalForm } from '@ant-design/pro-form';
import { handleAdd } from '@/utils/actions';
import { ActionType } from '@ant-design/pro-table';

type MegaCreateFormProps = {
  title: string,
  modalVisible: boolean,
  handleModalVisible: React.Dispatch<React.SetStateAction<boolean>>,
  children: React.ReactNode,
  actionRef: React.MutableRefObject<ActionType | undefined>,
  addEntityAction: any,
  postSubmit?: any,
}
const MegaCreateForm: FC<MegaCreateFormProps> = (
  props,
) => {
  const { title, modalVisible, handleModalVisible, actionRef, postSubmit, children, addEntityAction } = props;
  return (
    <ModalForm title={title}
               visible={modalVisible}
               onVisibleChange={handleModalVisible}
               modalProps={{
                 destroyOnClose: true,
               }}
               onFinish={async (value) => {
                 if (postSubmit != undefined) {
                   postSubmit(value);
                 }

                 const success = await handleAdd(value, addEntityAction);
                 if (success) {
                   handleModalVisible(false);
                   if (actionRef.current) {
                     await actionRef.current.reload();
                   }
                 }
               }}>
      {children}
    </ModalForm>
  );
};
export default MegaCreateForm;

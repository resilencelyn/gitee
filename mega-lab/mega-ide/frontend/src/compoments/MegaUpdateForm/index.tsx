import React, { FC} from 'react';
import { ModalForm } from '@ant-design/pro-form';
import { handleUpdate } from '@/utils/actions';
import { ActionType } from '@ant-design/pro-table';

type MegaUpdateFormProps = {
  title: string,
  modalVisible: boolean,
  handleModalVisible: (visible: boolean) => void,
  children: React.ReactNode,
  actionRef: React.MutableRefObject<ActionType | undefined>,
  currentEntity: BaseTableEntity | undefined
  postSubmit?: any,
  getEntityAction: any,
  updateEntityAction: any,
}
const MegaUpdateForm: FC<MegaUpdateFormProps> = (
  props,
) => {
  const {
    title,
    modalVisible,
    handleModalVisible,
    actionRef,
    postSubmit,
    children,
    getEntityAction,
    updateEntityAction,
    currentEntity,
  } = props;
  return (
    <ModalForm title={title}
               visible={modalVisible}
               onVisibleChange={handleModalVisible}
               modalProps={{
                 destroyOnClose: true,
               }}
               request={async () => {
                 return getEntityAction(currentEntity?.id);
               }}
               onFinish={async (value) => {
                 if (postSubmit != undefined) {
                   postSubmit(value);
                 }

                 const success = await handleUpdate(currentEntity?.id, value, updateEntityAction);
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
export default MegaUpdateForm;

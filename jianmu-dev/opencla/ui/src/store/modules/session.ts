import { ActionContext, Module } from 'vuex';
import { IState } from '@/model/modules/session';
import { IRootState } from '@/model';
import { ILoggingDto, ITokenVo } from '@/api/dto/auth';
import { getToken } from '@/api/auth';

/**
 * 命名空间
 */
export const namespace = 'session';

const DEFAULT_STATE: IState = {
  session: undefined,
};

export default {
  namespaced: true,
  state: {
    DEFAULT_STATE,
  },
  mutations: {
    mutate(
      state: IState,
      {
        session,
      }: {
        session: ITokenVo;
      },
    ) {
      state.session = session;
    },

    mutateDeletion(state: IState) {
      state.session = DEFAULT_STATE.session;
    },
  },
  actions: {
    async create(
      { commit }: ActionContext<IState, IRootState>,
      codeParam: ILoggingDto,
    ): Promise<void> {
      const session = await getToken({
        code: codeParam.code,
        thirdPartyType: codeParam.thirdPartyType,
        redirectUri: codeParam?.redirectUri,
      });
      commit('mutate', { session });
    },
  },
} as Module<IState, IRootState>;

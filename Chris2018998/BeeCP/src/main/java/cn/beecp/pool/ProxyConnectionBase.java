/*
 * Copyright(C) Chris2018998
 * Contact:Chris2018998@tom.com
 *
 * Licensed under GNU Lesser General Public License v2.1
 */
package cn.beecp.pool;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.concurrent.Executor;

import static cn.beecp.pool.PoolStaticCenter.*;

/**
 * rawConn connection wrapper
 *
 * @author Chris.Liao
 * @version 1.0
 */
abstract class ProxyConnectionBase extends ProxyBaseWrapper implements Connection {
    protected Connection raw;

    public ProxyConnectionBase(PooledConnection p) {
        super(p);
        raw = p.rawConn;
        p.proxyInUsing = this;
    }

    //***************************************************************************************************************//
    //                                             self-define methods(4)                                            //
    //***************************************************************************************************************//
    final void checkClosed() throws SQLException {
        if (this.isClosed) throw ConnectionClosedException;
    }

    synchronized final void registerStatement(ProxyStatementBase s) {
        this.p.registerStatement(s);
    }

    synchronized final void unregisterStatement(ProxyStatementBase s) {
        this.p.unregisterStatement(s);
    }

    //***************************************************************************************************************//
    //                                              override methods (11)                                            //
    //***************************************************************************************************************//
    public boolean isClosed() {
        return this.isClosed;
    }

    //call by borrower,then return PooledConnection to pool
    public final void close() throws SQLException {
        synchronized (this) {//safe close
            if (this.isClosed) return;
            this.isClosed = true;
            this.raw = CLOSED_CON;
            if (this.p.openStmSize > 0) this.p.clearStatement();
        }
        this.p.recycleSelf();
    }

    public final void setAutoCommit(boolean autoCommit) throws SQLException {
        //if (p.commitDirtyInd) throw DirtyTransactionException;
        this.raw.setAutoCommit(autoCommit);
        this.p.curAutoCommit = autoCommit;
        this.p.setResetInd(PS_AUTO, autoCommit != this.p.defaultAutoCommit);
    }

    public final void commit() throws SQLException {
        this.raw.commit();
        this.p.commitDirtyInd = false;
        this.p.lastAccessTime = System.currentTimeMillis();
    }

    public final void rollback() throws SQLException {
        this.raw.rollback();
        this.p.commitDirtyInd = false;
        this.p.lastAccessTime = System.currentTimeMillis();
    }

    public void setTransactionIsolation(int level) throws SQLException {
        this.raw.setTransactionIsolation(level);
        this.p.setResetInd(PS_TRANS, level != this.p.defaultTransactionIsolation);
    }

    public void setReadOnly(boolean readOnly) throws SQLException {
        this.raw.setReadOnly(readOnly);
        this.p.setResetInd(PS_READONLY, readOnly != this.p.defaultReadOnly);
    }

    public void setCatalog(String catalog) throws SQLException {
        this.raw.setCatalog(catalog);
        this.p.setResetInd(PS_CATALOG, !equalsString(catalog, this.p.defaultCatalog));
    }

    //for JDK1.7 begin
    public void setSchema(String schema) throws SQLException {
        this.raw.setSchema(schema);
        this.p.setResetInd(PS_SCHEMA, !equalsString(schema, this.p.defaultSchema));
    }

    public void setNetworkTimeout(Executor executor, int milliseconds) throws SQLException {
        if (this.p.supportNetworkTimeoutSet()) {
            this.raw.setNetworkTimeout(executor, milliseconds);
            this.p.setResetInd(PS_NETWORK, milliseconds != this.p.defaultNetworkTimeout);
        } else {
            throw DriverNotSupportNetworkTimeoutException;
        }
    }

    public void abort(Executor executor) throws SQLException {
        if (executor == null) throw new SQLException("executor can't be null");
        executor.execute(new ProxyConnectionCloseTask(this));
    }
    //for JDK1.7 end
}

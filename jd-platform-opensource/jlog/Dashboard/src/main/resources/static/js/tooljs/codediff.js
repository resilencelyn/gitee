/**
 * Created by hanxu3 on 2016/10/27.
 */
function initUI() {
    if (value == null) return;
    var target = document.getElementById("view");
    target.innerHTML = "";
    dv = CodeMirror.MergeView(target, {
        value: value,
        origLeft: panes == 3 && !collapse && !connect ? orig1 : null,
        orig: orig2,
        lineNumbers: true,
        lineWrapping:false,//自动换行
        smartIndent:true,//自动缩进
        styleActiveLine:false,//选择是否加亮
//            theme:"bespin",
        mode: {name: "javascript", json: true},
        size:{width: 1000, height: 1200},
        highlightDifferences: highlight,
        connect: connect,
        collapseIdentical: collapse
    });
//        dv.rightOriginal().setSize(null,800);
//        dv.editor().setSize(null,800);

    $("#view").width(1100);
    $("#view").height(900);
}

function mergeViewHeight(mergeView) {
    function editorHeight(editor) {
        if (!editor) return 0;
        return editor.getScrollInfo().height;
    }
    return Math.max(editorHeight(mergeView.leftOriginal()),
        editorHeight(mergeView.editor()),
        editorHeight(mergeView.rightOriginal()));
}

function resize(mergeView) {
    var height = mergeViewHeight(mergeView);
    for(;;) {
        if (mergeView.leftOriginal())
            mergeView.leftOriginal().setSize(null, height);
        mergeView.editor().setSize(null, height);
        if (mergeView.rightOriginal())
            mergeView.rightOriginal().setSize(null, height);

        var newHeight = mergeViewHeight(mergeView);
        if (newHeight >= height) break;
        else height = newHeight;
    }
    mergeView.wrap.style.height = height + "px";
}
(function () {
    'use strict';

    function isVsCodeWebView() {
        return /vscode/i.test(window.navigator.userAgent) || window.location.protocol === 'vscode-webview:';
    }

    function renderMermaid() {
        if (!window.mermaid) {
            return;
        }

        window.mermaid.initialize({
            startOnLoad: false,
            theme: 'default',
            securityLevel: 'strict'
        });

        window.mermaid.run({
            querySelector: '.mermaid'
        });
    }

    function loadMermaid() {
        if (window.mermaid) {
            renderMermaid();
            return;
        }

        if (isVsCodeWebView()) {
            return;
        }

        var script = document.createElement('script');
        script.src = 'assets/vendor/mermaid.min.js';
        script.onload = renderMermaid;
        document.body.appendChild(script);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', loadMermaid, { once: true });
        return;
    }

    loadMermaid();
}());
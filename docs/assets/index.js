(function () {
    'use strict';

    function isVsCodeWebView() {
        return /vscode/i.test(window.navigator.userAgent) || window.location.protocol === 'vscode-webview:';
    }

    function renderMermaid(scope) {
        if (!window.mermaid || !scope) {
            return;
        }

        window.mermaid.run({
            nodes: scope.querySelectorAll('.mermaid:not([data-processed])')
        });
    }

    function initializeMermaid() {
        if (!window.mermaid) {
            return;
        }

        window.mermaid.initialize({
            startOnLoad: false,
            theme: 'default',
            securityLevel: 'strict'
        });

        renderMermaid(document.querySelector('.tab-content.active'));
    }

    function loadMermaidScript(onReady) {
        if (window.mermaid) {
            onReady();
            return;
        }

        if (isVsCodeWebView()) {
            return;
        }

        var script = document.createElement('script');
        script.src = 'assets/vendor/mermaid.min.js';
        script.onload = onReady;
        document.body.appendChild(script);
    }

    function initializePage() {
        var chapterButtons = document.querySelectorAll('.tab-btn[data-tab]');
        var glossaryPopup = document.getElementById('glossaryPopup');
        var glossaryClose = document.getElementById('glossaryClose');
        var overlay = document.getElementById('overlay');
        var glossarySearch = document.getElementById('glossarySearch');

        function filterGlossary(query) {
            var rows = document.querySelectorAll('#glossaryBody tr');
            var normalizedQuery = query.toLowerCase();

            rows.forEach(function (row) {
                var text = row.textContent.toLowerCase();
                row.classList.toggle('hidden', normalizedQuery.length > 0 && text.indexOf(normalizedQuery) === -1);
            });
        }

        function openGlossary() {
            if (!glossaryPopup || !overlay || !glossarySearch) {
                return;
            }

            glossaryPopup.classList.add('show');
            overlay.classList.add('show');
            glossarySearch.focus();
        }

        function closeGlossary() {
            if (!glossaryPopup || !overlay || !glossarySearch) {
                return;
            }

            glossaryPopup.classList.remove('show');
            overlay.classList.remove('show');
            glossarySearch.value = '';
            filterGlossary('');
        }

        document.querySelectorAll('.tab-btn').forEach(function (button) {
            button.addEventListener('click', function () {
                if (button.dataset.action === 'glossary') {
                    openGlossary();
                    return;
                }

                closeGlossary();

                chapterButtons.forEach(function (item) {
                    item.classList.remove('active');
                });

                document.querySelectorAll('.tab-content').forEach(function (item) {
                    item.classList.remove('active');
                });

                button.classList.add('active');

                var target = document.getElementById(button.dataset.tab);
                if (!target) {
                    return;
                }

                target.classList.add('active');
                renderMermaid(target);
            });
        });

        if (glossaryClose) {
            glossaryClose.addEventListener('click', closeGlossary);
        }

        if (overlay) {
            overlay.addEventListener('click', closeGlossary);
        }

        if (glossarySearch) {
            glossarySearch.addEventListener('input', function () {
                filterGlossary(this.value);
            });
        }

        document.addEventListener('keydown', function (event) {
            if (event.key === 'Escape') {
                closeGlossary();
            }
        });

        loadMermaidScript(initializeMermaid);
    }

    if (document.readyState === 'loading') {
        document.addEventListener('DOMContentLoaded', initializePage, { once: true });
        return;
    }

    initializePage();
}());
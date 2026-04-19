(function () {
    'use strict';

    function renderMermaid(scope) {
        if (!window.mermaid || !scope) {
            return;
        }

        window.mermaid.run({
            nodes: scope.querySelectorAll('.mermaid:not([data-processed])')
        });
    }

    if (window.mermaid) {
        window.mermaid.initialize({
            startOnLoad: false,
            theme: 'default',
            securityLevel: 'strict'
        });
    }

    var chapterButtons = document.querySelectorAll('.tab-btn[data-tab]');

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

    renderMermaid(document.querySelector('.tab-content.active'));
}());